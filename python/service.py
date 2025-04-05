from fastapi import HTTPException
from python.models import Category, Expense
from python.schemas import CreateExpensePayload, GetExpensesPayload
from sqlalchemy.orm import Session
from sqlalchemy import func
from datetime import datetime

def upsert_expense(expense: CreateExpensePayload, session: Session):
    expense.date_of_expense = datetime.strptime(expense.date_of_expense, "%Y-%m-%d").date()
    existing_expense: Expense = session.query(Expense).filter(Expense.id == expense.expense_id).first()
    category: Category = session.query(Category).filter(func.lower(Category.name) == expense.category_name.lower()).first()
    if not category:
        raise HTTPException(status_code=404, detail="Category not found, please try another Category")
    if existing_expense:
        # Update existing expense
        existing_expense.name = expense.name
        existing_expense.description = expense.description
        existing_expense.amount_in_cents = (expense.amount) * 100
        existing_expense.date_of_expense = expense.date_of_expense
        existing_expense.category_id = category.id
        try:
            session.add(existing_expense)
            session.commit()
            session.refresh(existing_expense)
            return existing_expense
        except Exception as e:
            session.rollback()
            raise HTTPException(status_code=500, detail="Error updating expense: " + str(e))
    else:
        try:
            expense = Expense(
                name=expense.name,
                description=expense.description,
                amount_in_cents=(expense.amount) * 100,
                date_of_expense=expense.date_of_expense,
                category_id=category.id,
                user_id=expense.user_id,
                is_archived=False
            )
            session.add(expense)
            session.commit()
            session.refresh(expense)
            return expense
        except Exception as e:
            session.rollback()
            raise HTTPException(status_code=500, detail="Error creating expense: " + str(e))

def delete_expense(expense_id: int, session: Session):
    expense = session.query(Expense).filter(Expense.id == expense_id).first()
    if not expense:
        raise HTTPException(status_code=404, detail="Expense not found")
    
    session.delete(expense)
    session.commit()
    return {"detail": "Expense deleted successfully"}

def get_expenses(payload: GetExpensesPayload, session: Session):
    filters = []
    if payload.start_date:
        payload.start_date = datetime.strptime(payload.start_date, "%Y-%m-%d").replace(hour=0, minute=0, second=0, microsecond=0)
        filters.append(Expense.date_of_expense >= payload.start_date)
    if payload.end_date:
        payload.end_date = datetime.strptime(payload.end_date, "%Y-%m-%d").replace(hour=23, minute=59, second=59, microsecond=999999)
        filters.append(Expense.date_of_expense <= payload.end_date)
    if payload.category_name:
        category = session.query(Category).filter(Category.name == payload.category_name).first()
        if not category:
            raise HTTPException(status_code=404, detail="Category not found")
        filters.append(Expense.category_id == category.id)
    expenses = session.query(Expense, Category.name).join(Category, Category.id == Expense.category_id).filter(Expense.user_id == payload.user_id).filter(*filters).all()
    expenses_return = []
    for expense_data in expenses:
        expense: Expense = expense_data["Expense"]
        expense = {
            "id": expense.id,
            "name": expense.name,
            "description": expense.description,
            "amount_in_cents": expense.amount_in_cents,
            "date_of_expense": expense.date_of_expense,
            "category_id": expense.category_id,
            "user_id": expense.user_id,
            "is_archived": expense.is_archived
        }
        expense["category_name"] = expense_data[1]
        expense["amount"] = expense["amount_in_cents"] / 100
        expense["date_of_expense"] = expense["date_of_expense"].strftime("%Y-%m-%d")
        expenses_return.append(expense)
    return expenses_return

    
    
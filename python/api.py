from python import service
from python.constants import insert_categories_into_db
from python.models import Expense, Base, User
from python.schemas import CreateExpensePayload, ExpenseResponse, GetExpensesPayload
import uvicorn
from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from sqlalchemy import func

from fastapi import FastAPI, Depends
from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker
from sqlalchemy.orm import Session

app = FastAPI()
DATABASE_URL = "sqlite:///./expenses.db"
engine = create_engine(DATABASE_URL, connect_args={"check_same_thread": False})
SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine)
Base.metadata.create_all(bind=engine)
# insert_categories_into_db(session = SessionLocal())

app.add_middleware(
    CORSMiddleware,
    allow_origins=["http://localhost:3000"],
    allow_credentials=True,
    allow_methods=["*"], 
    allow_headers=["*"],  
)

@app.get("/get_user_id")
def get_user_id(name: str):
   session: Session = SessionLocal()
   name = name.lower().strip()
   user_name = session.query(User).filter(func.lower(User.name) == name.lower()).first()
   if user_name: 
       return {"user_id": user_name.id}

@app.post("/upsert_expense")
def upsert_expense(expense: CreateExpensePayload):
    session: Session = SessionLocal()
    expense = service.upsert_expense(expense=expense, session=session)
    return {"expense": expense}

@app.post("/get_expenses")
def get_expenses(get_expenses_payload: GetExpensesPayload):
    session: Session = SessionLocal()
    expenses = service.get_expenses(payload = get_expenses_payload, session=session)
    return {"expenses": expenses}

@app.delete("/{expense_id}")
def delete_expense(expense_id: int):
    session: Session = SessionLocal()
    response = service.delete_expense(expense_id=expense_id, session=session)
    return response

if __name__ == "__main__":
    uvicorn.run(app, host="127.0.0.1", port=8000)
from typing import Optional
from pydantic import BaseModel, Field

class GetExpensesPayload(BaseModel):
    user_id: int
    start_date: Optional[str] = None
    end_date: Optional[str] = None
    category_name: Optional[str] = None

class CreateExpensePayload(BaseModel):
    name: str
    description: Optional[str] = ""
    amount: float 
    date_of_expense: str
    user_id: int
    category_name: str
    expense_id: Optional[int] = None

class ExpenseResponse(BaseModel):
    expense_id: int
    name: str
    description: Optional[str] = ""
    amount_in_cents: int
    date_of_expense: str
    category_id: int
    user_id: int
    is_archived: bool = False
    class Config:
        orm_mode = True
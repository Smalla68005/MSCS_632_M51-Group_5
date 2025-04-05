#include <SQLiteCpp/SQLiteCpp.h>
#include <crow/json.h>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include "models.hpp"
#include "schemas.hpp"


Expense upsertExpense(SQLite::Database& db, const CreateExpensePayload& payload) {
    // Fetch Category
    SQLite::Statement category_query(db, "SELECT id FROM categories WHERE lower(name) = lower(?)");
    category_query.bind(1, payload.categoryName);

    if (!category_query.executeStep()) {
        throw std::runtime_error("Category not found");
    }
    int category_id = category_query.getColumn(0).getInt();
    Expense expense;
    int amount_in_cents = static_cast<int>(payload.amount * 100);
    if (payload.expenseId.has_value()) {
        // Update existing expense
        SQLite::Statement update_query(db,
            "UPDATE expenses SET name=?, description=?, amount_in_cents=?, date_of_expense=?, category_id=? WHERE id=?");
        
        update_query.bind(1, payload.name);
        update_query.bind(2, payload.description.value_or(""));
        update_query.bind(3, amount_in_cents);
        update_query.bind(4, payload.dateOfExpense);
        update_query.bind(5, category_id);
        update_query.bind(6, payload.expenseId.value());
        update_query.exec();

        expense.id = payload.expenseId.value();
    } else {
        SQLite::Statement insert_query(db,
            "INSERT INTO expenses (name, description, amount_in_cents, date_of_expense, category_id, user_id, is_archived) "
            "VALUES (?, ?, ?, ?, ?, ?, 0)");

        insert_query.bind(1, payload.name);
        insert_query.bind(2, payload.description.value_or(""));
        insert_query.bind(3, amount_in_cents);
        insert_query.bind(4, payload.dateOfExpense);
        insert_query.bind(5, category_id);
        insert_query.bind(6, payload.userId);
        insert_query.exec();

        expense.id = static_cast<int>(db.getLastInsertRowid());
    }

    expense.name = payload.name;
    expense.description = payload.description.value_or("");
    expense.amountInCents = amount_in_cents;
    expense.dateOfExpense = payload.dateOfExpense;
    expense.categoryId = category_id;
    expense.userId = payload.userId;
    expense.isArchived = false;

    return expense;
}

bool deleteExpense(SQLite::Database& db, int expense_id) {
    SQLite::Statement stmt(db, "DELETE FROM expenses WHERE id = ?");
    stmt.bind(1, expense_id);
    return stmt.exec() > 0;
}

std::vector<crow::json::wvalue> getExpenses(SQLite::Database& db, const GetExpensesPayload& payload) {
    std::ostringstream sql;
    sql << "SELECT e.id, e.name, e.description, e.amount_in_cents, e.date_of_expense, "
        << "e.category_id, e.user_id, e.is_archived, c.name "
        << "FROM expenses e "
        << "JOIN categories c ON e.category_id = c.id "
        << "WHERE e.user_id = ?";

    if (payload.startDate.has_value() && payload.startDate.value() != "") {
        sql << " AND e.date_of_expense >= '" << payload.startDate.value() << "'";
    }
    if (payload.endDate.has_value() && payload.endDate.value() != "") {
        sql << " AND e.date_of_expense <= '" << payload.endDate.value() << "23:59:59'";
    }
    if (payload.categoryName.has_value() && payload.categoryName.value() != "") {
        sql << " AND c.name = '" << payload.categoryName.value() << "'";
    }
    SQLite::Statement query(db, sql.str());
    query.bind(1, payload.userId);

    std::vector<crow::json::wvalue> results;

    while (query.executeStep()) {
        crow::json::wvalue item;
        item["id"] = query.getColumn(0).getInt();
        item["name"] = query.getColumn(1).getString();
        item["description"] = query.getColumn(2).getString();
        int amount_cents = query.getColumn(3).getInt();
        item["amount_in_cents"] = amount_cents;
        item["amount"] = std::round((amount_cents / 100.0) * 100) / 100.0;
        item["date_of_expense"] = query.getColumn(4).getString().substr(0, 10);
        item["category_id"] = query.getColumn(5).getInt();
        item["user_id"] = query.getColumn(6).getInt();
        item["is_archived"] = query.getColumn(7).getInt() != 0;
        item["category_name"] = query.getColumn(8).getString();
        results.push_back(item);
    }

    return results;
}

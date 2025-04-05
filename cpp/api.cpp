#include "crow.h"
#include "cors.h"
#include "SQLiteCpp/SQLiteCpp.h"
#include "schemas.hpp"
#include "models.hpp"
#include <vector>
#include <iostream>

// Forward declarations (assuming they're in another file)
ExpenseResponse upsertExpense(SQLite::Database &db, const CreateExpensePayload &payload);
std::vector<crow::json::wvalue> getExpenses(SQLite::Database &db, const GetExpensesPayload &payload);
bool deleteExpense(SQLite::Database &db, int expenseId);
// Parse JSON into CreateExpensePayload
CreateExpensePayload parseCreateExpensePayload(const crow::json::rvalue &json)
{
    CreateExpensePayload payload;
    payload.name = json["name"].s();
    if (json["amount"].t() == crow::json::type::String) {
        payload.amount = std::stof(json["amount"].s());
    } else {
        payload.amount = json["amount"].d();
    }
    payload.dateOfExpense = json["date_of_expense"].s();
    payload.userId = json["user_id"].i();
    payload.categoryName = json["category_name"].s();
    payload.description = json["description"].s();
    if (json.has("expense_id")) {
        try{
            payload.expenseId = json["expense_id"].i();
        }
        catch (const std::exception& e) {
            std::cout << "Caught exception: " << e.what() << std::endl;
        }
    }
    return payload;
}

// Parse JSON into GetExpensesPayload
GetExpensesPayload parseGetExpensesPayload(const crow::json::rvalue &json)
{
    GetExpensesPayload payload;
    payload.userId = json["user_id"].i();
    if (json.has("start_date"))
    {
        payload.startDate = json["start_date"].s();
    }
    if (json.has("end_date"))
    {
        payload.endDate = json["end_date"].s();
    }
    if (json.has("category_name"))
    {
        payload.categoryName = json["category_name"].s();
    }
    return payload;
}

// Handler for creating a new expense
crow::response upsertUserExpense(const crow::request &req)
{
    auto payloadJson = crow::json::load(req.body);
    if (!payloadJson)
    {
        return crow::response(400, "Invalid JSON");
    }
    CreateExpensePayload payload = parseCreateExpensePayload(payloadJson);
    SQLite::Database db("../../expenses.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    // Ensure the database connection is closed before returning
    std::cout << "Payload: " << payload.name << ", " << payload.amount << ", " 
              << payload.dateOfExpense << ", " << payload.userId << ", " 
              << payload.categoryName << ", " << std::endl;
    ExpenseResponse expense = upsertExpense(db, payload);

    crow::json::wvalue responseJson;
    responseJson["expense_id"] = expense.expenseId;
    responseJson["name"] = expense.name;
    responseJson["amount"] = expense.amountInCents;
    responseJson["date_of_expense"] = expense.dateOfExpense;
    responseJson["user_id"] = expense.userId;
    responseJson["category_name"] = expense.categoryId;
    responseJson["description"] = expense.description;

    
    return crow::response(200, responseJson);
}

// Handler for retrieving expenses
crow::response getUserExpenses(const crow::request &req)
{
    auto payloadJson = crow::json::load(req.body);
    if (!payloadJson)
    {
        return crow::response(400, "Invalid JSON");
    }

    GetExpensesPayload payload = parseGetExpensesPayload(payloadJson);

    SQLite::Database* db = new SQLite::Database("../../expenses.db", SQLite::OPEN_READWRITE);
    std::vector<crow::json::wvalue> expenses = getExpenses(*db, payload);

    crow::json::wvalue response_json;
    response_json["expenses"] = crow::json::wvalue::list(expenses);
    delete db;
    return crow::response(200, response_json);
}

crow::response getUserId(const crow::request &req)
{
    crow::json::wvalue response_json;
    SQLite::Database* db = new SQLite::Database("../../expenses.db", SQLite::OPEN_READWRITE);
    std::string userName = req.url_params.get("name");
    SQLite::Statement user_query(*db, "SELECT id FROM users WHERE lower(name) = lower(?)");
    user_query.bind(1, userName);
    if (user_query.executeStep())
    {
        response_json["user_id"] = user_query.getColumn(0).getInt();
    }
    else
    {
        delete db;
        return crow::response(404, "User not found");
    }
    delete db;
    return crow::response(200, response_json);
}

crow::response deleteUserExpense(const crow::request &req, int expense_id) {
    // SQLite::Database db("../../expenses.db", SQLite::OPEN_READWRITE);
    SQLite::Database* db = new SQLite::Database("../../expenses.db", SQLite::OPEN_READWRITE);
    crow::json::wvalue response_json;
    bool success = deleteExpense(*db, expense_id);
    
    if (success) {
        delete db; // Close the database connection
        response_json["message"] = "Expense deleted successfully";
        return crow::response(200, response_json);
    } else {
        delete db;
        return crow::response(404, "Expense not found");
    }
}

int main()
{
    crow::App<crow::CORSHandler> app;

    auto &cors = app.get_middleware<crow::CORSHandler>();

    CROW_ROUTE(app, "/upsert_expense").methods("POST"_method)(upsertUserExpense);
    CROW_ROUTE(app, "/get_expenses").methods("POST"_method)(getUserExpenses);
    CROW_ROUTE(app, "/get_user_id").methods("GET"_method)(getUserId);
    CROW_ROUTE(app, "/<int>").methods("DELETE"_method)(deleteUserExpense);
    app.port(8000).run();
}

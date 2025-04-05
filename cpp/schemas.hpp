#pragma once
#include <string>
#include <optional>

struct GetExpensesPayload {
    int userId;
    std::optional<std::string> startDate;
    std::optional<std::string> endDate;
    std::optional<std::string> categoryName;
};

struct CreateExpensePayload {
    std::string name;
    std::optional<std::string> description;
    double amount;
    std::string dateOfExpense;
    int userId;
    std::string categoryName;
    std::optional<int> expenseId;

    // Constructor to initialize default values for optional fields
    CreateExpensePayload()
        : description(""), expenseId(std::nullopt) {}
};

struct ExpenseResponse {
    int expenseId;
    std::string name;
    std::string description = "";
    int amountInCents;
    std::string dateOfExpense;
    int categoryId;
    int userId;
    bool isArchived = false;

    // Constructor to set default values for optional fields
    ExpenseResponse()
        : description(""), isArchived(false) {}
};

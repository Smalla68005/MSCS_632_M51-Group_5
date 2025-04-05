#include <SQLiteCpp/SQLiteCpp.h>
#include <string>
#include <vector>

struct User {
    int id;
    std::string name;
};

struct Expense {
    int id;
    std::string name;
    std::string description;
    int amountInCents;  // Store amount in cents
    std::string dateOfExpense;
    int categoryId;
    int userId;
    bool isArchived;

    Expense() = default;

    Expense(int id, const std::string& name, const std::string& description, int amountInCents, 
            const std::string& dateOfExpense, int categoryId, int userId, bool isArchived)
        : id(id), name(name), description(description), amountInCents(amountInCents),
          dateOfExpense(dateOfExpense), categoryId(categoryId), userId(userId), isArchived(isArchived) {}
};

struct Category {
    int id;
    std::string name;
};

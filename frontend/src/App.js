import React, { useEffect, useState } from "react";
import axios from "axios";
import "./App.css"; // Assuming you have some CSS for styling

function App() {
  const SERVER_URL = "http://localhost:8000"; // Replace with your server URL
  const [userName, setUserName] = useState("");
  const [userId, setUserId] = useState(null); // Store user ID
  const [categories] = useState([
    "Food",
    "Transport",
    "Entertainment",
    "Health",
    "Education",
    "Housing",
    "Clothing",
    "Utilities",
    "Insurance",
    "Miscellaneous",
  ]);
  const [filteredExpenses, setFilteredExpenses] = useState([]);
  const [dateFrom, setDateFrom] = useState("");
  const [dateTo, setDateTo] = useState("");
  const [selectedCategory, setSelectedCategory] = useState(""); // Category state
  const [modalOpen, setModalOpen] = useState(false); // State for modal visibility
  const [expenseData, setExpenseData] = useState({
    expense_id: null,
    name: "",
    description: "",
    amount: "",
    category_name: "",
    date_of_expense: "",
  });
  const [triggerFetch, setTriggerFetch] = useState(false);

  useEffect(() => {
    const fetchExpenses = async () => {
      try {
        const response = await axios.post(`${SERVER_URL}/get_expenses`, {
          user_id: userId,
          start_date: dateFrom,
          end_date: dateTo,
          category_name: selectedCategory,
        });
        setFilteredExpenses(response.data);
      } catch (error) {
        console.error("Error fetching expenses:", error);
      }
    };
    if (userId) {
      fetchExpenses();
    }
  }, [userId, triggerFetch]);

  const handleSubmit = () => {
    const updatedExpenseData = {
      ...expenseData,
      user_id: userId,
    };

    console.log("Expense data to be saved:", updatedExpenseData);
    axios
      .post(`${SERVER_URL}/upsert_expense`, updatedExpenseData)
      .then((response) => {
        console.log("Expense saved:", response.data);
        setModalOpen(false); // Close the modal
        setExpenseData({}); // Reset the form
        setTriggerFetch((prev) => !prev); // Trigger re-fetch of expenses
      })
      .catch((error) => {
        console.error("Error saving expense:", error);
      });
  };

  const handleExpenseChange = (e) => {
    const { name, value } = e.target;
    setExpenseData((prevData) => ({
      ...prevData,
      [name]: value,
    }));
  };

  const handleCategoryChange = (e) => {
    setSelectedCategory(e.target.value);
  };

  const handleUserNameChange = (e) => {
    setUserName(e.target.value);
  };

  const handleGetUserId = async () => {
    try {
      const response = await axios.get(`${SERVER_URL}/get_user_id`, {
        params: { name: userName },
      });
      setUserId(response.data.user_id);
    } catch (error) {
      console.error("Error fetching user ID:", error);
    }
  };

  // Handle changes in the date filter inputs
  const handleDateFromChange = (e) => {
    setDateFrom(e.target.value);
  };

  const handleDateToChange = (e) => {
    setDateTo(e.target.value);
  };

  // Handle filtering expenses based on dates and category
  const handleFilterExpenses = () => {
    setTriggerFetch((prev) => !prev); // Trigger re-fetch of expenses
  };

  const handleUpsertExpense = (expense) => {
    setExpenseData({
      expense_id: expense.id,
      name: expense.name,
      description: expense.description,
      amount: expense.amount, // Convert to cents
      category_name: expense.category_name,
      date_of_expense: expense.date_of_expense,
    });
    setModalOpen(true); // Open the modal for editing
  };

  // Handle delete expense
  const handleDeleteExpense = async (expenseId) => {
    try {
      await axios.delete(`${SERVER_URL}/${expenseId}`);
      setTriggerFetch((prev) => !prev); // Trigger re-fetch of expenses
    } catch (error) {
      console.error("Error deleting expense:", error);
    }
  };

  const handleModalMount = () => {
    setModalOpen(true);
  };

  return (
    <div className="App wrapper">
      <div className="header">
        <h1>Expense Tracker</h1>
        <div className="user-wrapper">
          <input
            type="text"
            placeholder="your name"
            value={userName}
            onChange={handleUserNameChange}
          />
          <button onClick={handleGetUserId}>Get User ID</button>
          {userId && <p>User ID: {userId}</p>}
        </div>
      </div>

      <div className="filters">
        <div className="date-filters">
          <input
            type="date"
            placeholder="From"
            value={dateFrom}
            onChange={handleDateFromChange}
          />
          <input
            type="date"
            placeholder="To"
            value={dateTo}
            onChange={handleDateToChange}
          />
        </div>
        <div className="category-filter">
          <div>Filter by Category:</div>
          <select
            id="category"
            value={selectedCategory}
            onChange={handleCategoryChange}
          >
            <option value="">All Categories</option>
            {categories.map((category) => (
              <option key={category} value={category}>
                {category}
              </option>
            ))}
          </select>
        </div>
        <button onClick={handleFilterExpenses}>Filter Expenses</button>
      </div>
      <div className="table-header">
        <div className="add-expense">
          <h2>Expenses</h2>
          <button onClick={() => handleModalMount()}>Add Expense</button>
        </div>
        <div className="modal-total">
          {filteredExpenses.length > 0 && (
            <h3>
              Total amount spent for selected filters :{" "}
              {filteredExpenses.reduce(
                (acc, expense) => acc + expense.amount,
                0
              )}
            </h3>
          )}
        </div>
      </div>
      <table>
        <thead>
          <tr>
            <th>Name</th>
            <th>Description</th>
            <th>Amount</th>
            <th>Date</th>
            <th>Category</th>
            <th>Actions</th>
          </tr>
        </thead>
        <tbody>
          {filteredExpenses.map((expense) => (
            <tr key={expense.id}>
              <td>{expense.name}</td>
              <td>{expense.description}</td>
              <td>{expense.amount}</td>
              <td>{expense.date_of_expense}</td>
              <td>{expense.category_name}</td>
              <td className="actions">
                <button onClick={() => handleUpsertExpense(expense)}>
                  Edit
                </button>
                <button
                  className="delete-button"
                  onClick={() => handleDeleteExpense(expense.id)}
                >
                  Delete
                </button>
              </td>
            </tr>
          ))}
        </tbody>
      </table>
      {modalOpen && (
        <div className="modal-wrapper">
          <div className="modal">
            <div className="modal-header">
              <h2>Add/Edit Expense</h2>
              <button onClick={() => setModalOpen(false)}>Close</button>
            </div>
            <div className="modal-content">
              <div className="form-group">
                <div className="label">Expense Name</div>
                <input
                  type="text"
                  name="name"
                  value={expenseData.name}
                  onChange={handleExpenseChange}
                  placeholder="expense name"
                />
              </div>
              <div className="form-group">
                <div className="label">Description</div>
                <input
                  type="text"
                  name="description"
                  value={expenseData.description}
                  onChange={handleExpenseChange}
                  placeholder="expense description"
                />
              </div>
              <div className="form-group">
                <div className="label">Amount</div>
                <input
                  type="floatInput"
                  name="amount"
                  value={expenseData.amount}
                  onChange={handleExpenseChange}
                  placeholder="amount"
                />
              </div>
              <div className="form-group">
                <div className="label">Category</div>
                <select
                  name="category_name"
                  value={expenseData.category_name}
                  onChange={handleExpenseChange}
                >
                  <option value="">Select a category</option>
                  {categories.map((category) => (
                    <option key={category} value={category}>
                      {category}
                    </option>
                  ))}
                </select>
              </div>
              <div className="form-group">
                <div className="label">Date of Expense</div>
                <input
                  type="date"
                  name="date_of_expense"
                  value={expenseData.date_of_expense}
                  onChange={handleExpenseChange}
                />
              </div>
              <button onClick={handleSubmit}>Save Expense</button>
            </div>
          </div>
        </div>
      )}
    </div>
  );
}

export default App;

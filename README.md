# MSCS_632_M51-Group_5

# Expense Tracker

## Overview

**Expense Tracker** is an application designed to help users record, view, and categorize their expenses. The app allows users to:

- Record expenses with specific categories and dates.
- Filter expenses by date or category.
- Calculate the total of all expenses.

## Team Members

- **Suyog Bikram Malla**
- **Tanmay Siwach**
- **Syed Noor Ul Hassan**
- **Muhammad Nouman Anwar**

## Build and Run
Both Python and C++ run on the same PORT @ 8000. Only one server can be run at one time.
### Python
1. run pip install -r requirements_python.txt in MSCS_632_M51-GROUP_5 (root) folder
2. run uvicorn python.api:app --reload

### C++
1. Clone Crow from this repo -> https://github.com/CrowCpp/Crow
2. Clone SQLiteCPP from this repo -> https://github.com/SRombauts/SQLiteCpp
    cd SQLiteCPP -> cd build -> cmake .. -> make -> sudo make install 
3. brew install asio
4. cd cpp
5. cd build
6. cmake ..
7. make
8. ./expense_api

### ReactJS
1. cd frontend
2. npm install
3. npm start



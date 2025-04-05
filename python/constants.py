from python.models import Category


def insert_categories_into_db(session):
    categories = [
        "Food",
        "Transport",
        "Entertainment",
        "Health",
        "Education",
        "Housing",
        "Clothing",
        "Utilities",
        "Insurance",
        "Miscellaneous"
    ]

    for category in categories:
        new_category = Category(name=category)
        session.add(new_category)
    
    session.commit()

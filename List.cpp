#include "List.hpp"

List::List(Node<string>* _pHead, int _size)
{
	this->size = _size;
	this->pHead = this->pTail = _pHead;
	while (pHead != nullptr && pTail->getNext() != nullptr) {
		pTail = pTail->getNext();
	}
}

List::List(int groceryCount, mt19937& rng)
{
	this->size = 0;
	this->pHead = this->pTail = nullptr;
	populateWithGroceries(groceryCount, rng);
}

List::List(const List& copy)
{
	this->size = 0;
	this->pHead = this->pTail = nullptr;
	Node<string>* pCopyCurr = copy.pHead;
	while (pCopyCurr != nullptr) {
		this->insertAtEnd(pCopyCurr->getData());
		pCopyCurr = pCopyCurr->getNext();
	}
}

List& List::operator=(const List& copy)
{
	this->size = 0;
	Node<string>* pCopyCurr = copy.pHead;
	while (pCopyCurr != nullptr) {
		insertAtEnd(pCopyCurr->getData());
		pCopyCurr = pCopyCurr->getNext();
	}
	return *this;
}

List::~List()
{	//recursive or iterative deletion based on list size
	if (size < 100) delete pHead;
	else {
		while (!isEmpty()) {
			Node<string>* toDel = pHead;
			pHead = pHead->getNext();
			toDel->setNext(nullptr);
			delete toDel;
		}
		pTail = nullptr;
	}
}

void List::populateWithGroceries(int groceryCount, mt19937& rng)
{
	for (int i = 0; i < groceryCount; ++i) {
		insertAtEnd(genGrocery(rng));
	}
}

bool List::isEmpty() const
{
	return pHead == nullptr;
}

bool List::insertAtFront(const string& data)
{
	Node<string>* newNode = new Node<string>(data);
	if (newNode == nullptr) return false;
	newNode->setNext(pHead);
	pHead = newNode;
	++size;
	return true;
}

bool List::insertAtEnd(const string& data)
{
	Node<string>* newNode = new Node<string>(data);
	if (newNode == nullptr) return false;
	if (isEmpty()) {
		pHead = pTail = newNode;	
	}
	else {
		pTail->setNext(newNode);
		pTail = newNode;
	}
	++size;
	return true;
}

string List::listToString(void) const
{
	Node<string>* pCurr = pHead;
	string out;
	while (pCurr != nullptr) {
		out += "\t\t";
		out += pCurr->getData();
		out += "\n";
		pCurr = pCurr->getNext();
	}
	return out;
}

int List::getSize(void) const
{
	return size;
}

Node<string>* List::getHead(void) const
{
	return pHead;
}

string genGrocery(mt19937& rng)
{
	string groceryTypes[] = { //sample groceries taken from https://portal.ct.gov/-/media/DMHAS/SkillBuilding/TheUltimateGroceryShoppingListpdf.pdf
		/* Vegetables			*/	"Asparagus", "Beets", "Broccoli", "Carrots", "Cauliflower", "Celery", "Corn", "Cucumbers", "Greens", "Lettuce", "Mushrooms", "Onions", "Peppers", "Potatoes", "Spinach", "Sprouts", "Squash", "Tomatoes", "Zucchini",
		/* Fruits				*/	"Apples", "Avocado", "Bananas", "Berries", "Cherries", "Grapes", "Kiwis", "Lemons", "Limes", "Melons", "Oranges", "Peaches", "Pears", "Plums",
		/* Canned Foods			*/	"Applesauce", "Baked Beans", "Beans", "Mixed Fruit", "Mixed Veggies", "Olives", "Pasta Sauce", "Pickles", "Refried Beans", "Canned Tuna", "Soup",
		/* Sauces				*/	"BBQ Sauce", "Hot Sauce", "Salsa", "Soy Sauce", "Steak Sauce", "Syrup", "Worcestershire Sauce",
		/* Various				*/	"Bottled Water", "Bullion Cubes", "Cereal", "Coffee", "Gravy", "Honey", "Jelly", "Ketchup", "Mac & Cheese", "Mayonnaise", "Mustard", "Pancake Mix", "Peanut Butter", "Ramen", "Soda", "Tea", "White Rice", "Wild Rice",
		/* Spices & Herbs		*/	"Basil", "Black Pepper", "Cilantro", "Cinnamon", "Garlic", "Oregano", "Parsely", "Red Pepper", "Salt", "Vanilla Extract",
		/* Oils & Vinnegars		*/	"Apple Cider Vinegar", "Basalmic Vinegar", "Salad Dressing", "Olive Oil", "Vegetable Oil", "White Vinegar",
		/* Refrigerated Items	*/	"Chip Dip", "Eggs", "Juice", "Tofu", "Tortillas",
		/* Dairy				*/	"Butter", "Half & Half", "Heavy Cream", "Margarine", "Milk", "Sour Cream", "Whipped Cream", "Yogurt",
		/* Cheese				*/	"Cheddar", "Cottage Cheese", "Cream Cheese", "Feta", "Mozzarella", "Parmesan", "Pepperjack", "Provolone", "Ricotta", "Shredded Cheese", "Sliced Cheese", "Swiss Cheese",
		/* Frozen				*/	"Burritos", "Desserts", "Fish Sticks", "Ice Cream", "Juice", "Pizza", "Popsicles", "Fries", "Tater Tots", "Sorbet",
		/* Meat					*/	"Bacon", "Beef", "Chicken", "Ground Beef", "Ground Turkey", "Ham", "Hot Dogs", "Lunchmeat", "Pork", "Sausage", "Steak", "Turkey",
		/* Seafood				*/	"Catfish", "Cocktail Sauce", "Crab", "Halibut", "Oysters", "Salmon", "Shrimp", "Tilapia", "Tuna",
		/* Baked Goods			*/	"Bagels", "Bread", "Buns", "Cake", "Cookies", "Crackers", "Croissants", "Donuts", "Pastries", "Pie", "Pita", "Rolls", "Sliced Bread",
		/* Baking				*/	"Baking Powder", "Baking Soda", "Bread Crumbs", "Brownie Mix", "Brown Sugar", "Cake Decorations", "Cake Icing", "Cake Mix", "Chocolate Chips", "Cocoa", "Flour", "Oatmeal", "Pie Shell", "Powdered Sugar", "Shortening", "Sugar", "Yeast",
		/* Snacks				*/	"Candy", "Chips", "Cookies", "Dried Fruit", "Granola Bars", "Gum", "Nuts", "Popcorn", "Pudding", "Pretzels", "Tortilla Chips",
		/* Personal Care		*/	"Antiperspirant", "Bath soap", "Conditioner", "Condoms", "Cosmetics", "Deodorant", "Facial Cleanser", "Facial Tissues", "Floss", "Hair Gel", "Hair Spray", "Hand Soap", "Lip Balm", "Lotion", "Mouthwash", "Q-Tips", "Razors", "Shampoo", "Shaving Cream", "Toilet Paper", "Toothpaste",
		/* Medicine				*/	"Allergy Medicine", "Antidiarrheal Medicine", "Aspirin", "Antacid", "Band-Aids", "Cold Medicine", "Feminine Products", "Prescription Medicine", "Sinus Medicine", "Vitamins",
		/* Kitchen				*/	"Aluminum Foil", "Coffee Filters", "Dish Soap", "Dishwasher Soap", "Disposable Cups", "Disposable Cutlery", "Disposable Plates", "Freezer Bags", "Napkins", "Non-Stick Spray", "Paper Towels", "Plastic Wrap", "Sandwich Bags", "Sponges", "Wax Paper",
		/* Cleaning Products	*/	"Air Freshener", "Bleach", "Dryer Sheets", "Fabric Softener", "Garbage Bags", "Glass Cleaner", "Laundry Detergent", "Mop Head", "Spray Polish", "Vacuum Bag",
		/* Pets					*/	"Cat Food", "Cat Litter", "Cat Treats", "Dog Food", "Dog Treats", "Flea Shampoo", "Pet Shampoo",
		/* Baby					*/	"Baby Food", "Baby Formula", "Bottles", "Diapers", "Wipes",
		/* Office				*/	"Envelopes", "Glue", "Notepads", "Paper", "Pencils", "Pens", "Scotch Tape",
		/* Alcohol				*/	"Beer", "Champagne", "Club Soda", "Gin", "Malt", "Red Wine", "Rum", "Sake", "Tonic", "Whiskey", "White Wine", "Vodka",
		/* Other				*/	"Batteries", "Bug Spray", "Candles", "Charcoal", "Flowers", "Cards", "Light Bulbs", "Sunscreen"
	};
	std::uniform_int_distribution<> possibleGroceries(0, 276);
	return groceryTypes[possibleGroceries(rng)];
}

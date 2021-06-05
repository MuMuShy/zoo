#pragma once
#include<iostream>
#include<vector>
#include<string>

class Animal {
public:
	Animal(std::string inName, int inEnergy = 100) //名稱跟初始體力
		: Name(inName), hungry(0), energy(inEnergy) {};
	//傳入消耗量，回傳飢餓度
	int feed(int amount) {
		hungry -= amount;
		hungry = std::max(0, hungry);
		return 1;
	}
	virtual int exercise(int times) = 0; //傳入次數，回傳所賺的錢
	virtual bool rest(int& food, int& water) = 0; //需要傳入一些資訊方便你調整動物園食物量 水量...
	int drink(int amount) { return true; };
	friend std::ostream& operator<<(std::ostream& os, const Animal& a) {
		os << &a;
	}
	friend std::ostream& operator<<(std::ostream& os, const Animal* a) {
		return os << a->Name << " (hungry: " << a->hungry << ", energy : " << a->energy << ")" << std::endl;
	}
	void print() {
		std::cout << this;
	}
protected:
	std::string Name;
	int hungry;	//hungry index (initial = 0, upper bound is varying)
	int energy; //energy index (initial = 100)
	void log_eat(int f) {
		std::cout << "[LOG] " << Name << " eat " << f << " food" << std::endl;
	}
	void log_drink(int f) {
		std::cout << "[LOG] " << Name << " drink " << f << " water" << std::endl;
	}
	void log_rest(int food, int water, int enrgy) {
		std::cout << "[LOG] " << Name << " take a rest, eat " << food << " food & drink " << water << " water, gain " << enrgy << std::endl;
	}

};


class Zoo {
public:
	~Zoo() {
		for (const auto& u : house) {
			delete u;
		}
	}

	Animal*& operator[](int i) {
		return house[i];
;	}
	void operator+=(Animal* a) {
		AddAnimal(a);
	}
	friend std::ostream& operator<<(std::ostream& os, const Zoo& a) {
		return os << &a;
	}
	friend std::ostream& operator<<(std::ostream& os, const Zoo* a) {
		os << "[INFO] Food: " << a->foodCount << ", Water : " << a->waterCount << ", Foud$ : " << a->deposit << "$";
		return os;
	}
	Zoo(size_t size = 5) : foodCount(500), waterCount(100), deposit(100), sizeLimit(size) {
		house.reserve(size);
	}
	int Run(int quantity=1) {
		bool flg = false;
		if (waterCount < 100000) waterCount += 100000;
		FeedWater();
		for (const auto& a : house) {
			int r = a->exercise(10);
			deposit += r;
			flg = flg || r;
		}
		if (!flg) {
			AskRest();
			if (!FeedFood(10)) {
				getMoreFood(30);
			}
		}
		
		return 0;
	}
	bool FeedFood(int quantity=1) {
		std::cout << "[LOG] Feeding food...\n";
		int fed = 0;
		for (const auto& u : house) {
			if (foodCount < quantity) {
				std::cout << "[WARNING] Insufficient food !!!\n";
				std::cout << "[LOG] total consume " << fed << " food.\n";
				return false;
			}
			if (u->feed(quantity)) { fed += quantity; foodCount -= quantity; }
		}
		std::cout << "[LOG] total consume " << fed << " food.\n";
		return true;
	}
	bool FeedWater(int quantity = 1) {
		std::cout << "[LOG] Feeding water...\n";
		int fed = 0;

		for (const auto& u : house) {
			if (waterCount < 10) {
				std::cout << "[WARNING] Insufficient water !!!\n";
				std::cout << "[LOG] total consume " << fed << " water.\n";
				return false;
			}
			if (u->feed(10)) {
				fed += 10;
				waterCount -= 10;
			}
		}
		std::cout << "[LOG] total consume " << fed << " water.\n";
		return true;

	}
	bool AskRest() {
		// return: all aminals fails to rest
		std::cout << "[LOG] ask all animals to rest.\n";
		bool ret = false;
		for (const auto& u : house) {
			ret |= u->rest(foodCount, waterCount);
		}
		return ret;
	}
	void Listing() { //印出所有動物資訊
		for (std::vector<Animal*>::iterator iter = house.begin(); iter != house.end(); ++iter) {
			(*iter)->print();
		}
	}
	bool AddAnimal(Animal* a) {
		if (sizeLimit <= getCount()) {
			std::cout << "[WARNING] Too many animals !!!\n";
		}
		house.emplace_back(a);
		return true;
	}
	int getCount() { return house.size(); }
	bool isBankrupt() {
		if (foodCount == 0 && deposit == 0)
			return true;
		//you may update this function
		return false;
	}
	int getMoreFood(int amount) { // try to buy food
		int n = std::min(amount, deposit);
		if (n != amount) {
			std::cout << "[WARNING] Insufficient funds !!!\n";
		}
		std::cout << "[LOG] Transfer $"<< n <<" to "<<(n*2)<<" food";
		deposit -= n;
		foodCount += n * 2;
		return true;
	}
	//operator+=
	//operator[]
	//operator<< //[INFO] Food: ###, Water: ###, Foud$: ###$

private:
	std::vector<Animal*> house;
	int foodCount; //動物園食物 沒食物可以用存款轉換
	int waterCount; //動物園水 每RUN會增加100;
	int deposit; //動物園存款 動物運動可增加
	const int sizeLimit; //動物園設計上限(初次創建時設定)可超過但追加動物會警告
};


class horse : public Animal {
public:
	horse(std::string inName, int inEnergy = 100) : Animal(inName, inEnergy) {}
	bool rest(int& food, int& water) {
		constexpr int WATER_COST = 10;
		if (water < WATER_COST) return false;
		water -= WATER_COST;
		drink(WATER_COST);
		return true;
	}
	int feed(int n) {
		__super::feed(n);
		energy += n * 2;
		
		return true;
	}
	int drink(int n) {
		__super::drink(n);
		energy += n;
		return true;
	}

	int exercise(int times) {
		int ret = 0;
		while (times--) {
			if (isHungry() && energy > 50) {
				energy -= 50;
				ret += 25;
			}
			else if (!isHungry() && energy >= 35) {
				hungry += 35;
				energy -= 35;
				hungry = std::min(hungry, 100);
				ret += 50;
			}
		}

		return ret;
	}
private:
	bool isHungry() {
		return hungry >= 100;
	}
};

class pig : public Animal {
public:
	pig(std::string inName, int inEnergy = 100) : Animal(inName, inEnergy) {}
	bool rest(int& food, int& water) {
		constexpr int FOOD_COST = 10;
		if (food < FOOD_COST) return false;
		food -= FOOD_COST;
		return true;
	}
	int feed(int n) {
		__super::feed(n);
		if (n > hungry) energy += (n - hungry) * 2;
		return true;
	}
	int drink(int n) {
		__super::drink(n);
		energy += n;
	}

	int exercise(int times) {
		int ret = 0;
		while (times--) {
			if (isHungry()) {
				ret += 0;
				break;
			}
			else if (energy >= 35) {
				hungry += 20;
				energy -= 35;
				hungry = std::min(hungry, 50);
				ret += 100;
			}
			else break;
		}

		return ret;
	}
private:
	bool isHungry() {
		return hungry >= 50;
	}
};

class human : public Animal {
public:
	human(std::string inName, int inEnergy = 100) : Animal(inName, inEnergy) {}
	bool rest(int& food, int& water) {
		if (energy < 10) return false;
		energy -= 10;
		
		if (water < 20 || food < 10) return false;
		water -= 20;
		food -= 10;
		energy += 20;
		return true;
	}
	int feed(int n) {
		__super::feed(n);
		energy += n;
		return true;
	}
	int drink(int n) {
		__super::drink(n);
		energy += n * 2;
	}

	int exercise(int times) {
		int ret = 0;
		while (times--) {
			if (energy < 10) break;
			energy -= 10;
			if (isHungry()) {
				ret += 0;
				break;
			}
			else if (energy >= 35) {
				hungry += 20;
				energy -= 35;
				hungry = std::min(hungry, 150);
				ret += 100;
			}
			else break;
		}

		return ret;
	}
private:
	bool isHungry() {
		return hungry >= 150;
	}
};
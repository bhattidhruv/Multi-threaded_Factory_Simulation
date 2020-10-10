//Dhruv G Bhatti
//centralized locking system

#include<thread>
#include<iostream>
#include<time.h>
#include<chrono>
#include<mutex>
#include<condition_variable>
#include<vector>
#include <random>
#include<map>
#include<unordered_map>
#include <fstream>

using namespace std;

//*******Global Variables*******//

//to write into text file
ofstream mout("productionPlant.txt");
std::chrono::time_point<std::chrono::steady_clock>globalTime;
// number of rows(r) and columns(c) in load order and pickup order list 
int r = 50, c = 4, numOfProducts = 0;

//2-D vector with all the randomly generated legal combination of load order
vector<vector<int>> loadOrderList(r, vector<int>(c, 0));
//2-D vector with all the randomly generated legal combination of pickup order
vector<vector<int>> pickupOrderList(r, vector<int>(c, 0));
int seed;

//*******Global Variables*******//


//Central Lock class
class blocking_queue {
private:
	//mutex lock for protecting Shared Buffer
	mutex access;
	//condition variable load::for part workers && unload for product workers
	condition_variable load, unload;
	//Shared Buffer
	vector<int> sharedBuffer{ 0,0,0,0 };

public:

	//function to get the lock and loadinto buffer
	void putIn(vector<int> loadorder, int i, int PartWorkerIteration, bool& timed_out) {
		vector<int> tempOrder = loadorder;
		{
			unique_lock<mutex> load_unlock(access);
			std::chrono::steady_clock::time_point accStartTime = std::chrono::steady_clock::now();
			if (sharedBuffer == vector<int>{6, 5, 4, 3}) {
				load.wait(load_unlock);
			}

			/*print status before putting in buffer
			console output*/
			/*cout << endl;
			cout << "Current Time : " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - globalTime).count() << " us" << endl;
			cout << "Part_Worker ID : " << i << endl;
			cout << "Iteration : " << PartWorkerIteration << endl;
			cout << "Status : New Load Order " << endl;
			cout << "Accumulated Wait Time : " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - accStartTime).count() << " us" << endl;
			cout << "Buffer State : " << "( " << sharedBuffer[0] << "," << sharedBuffer[1] << "," << sharedBuffer[2] << "," << sharedBuffer[3] << " )" << endl;
			cout << "Load Order : " << "[ " << loadorder[0] << "," << loadorder[1] << "," << loadorder[2] << "," << loadorder[3] << " ]" << endl;*/

			mout << endl;
			mout << "Current Time : " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - globalTime).count() << " us" << endl;
			mout << "Part_Worker ID : " << i << endl;
			mout << "Iteration : " << PartWorkerIteration << endl;
			mout << "Status : New Load Order " << endl;
			mout << "Accumulated Wait Time : " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - accStartTime).count() << " us" << endl;
			mout << "Buffer State : " << "( " << sharedBuffer[0] << "," << sharedBuffer[1] << "," << sharedBuffer[2] << "," << sharedBuffer[3] << " )" << endl;
			mout << "Load Order : " << "[ " << loadorder[0] << "," << loadorder[1] << "," << loadorder[2] << "," << loadorder[3] << " ]" << endl;

			load_buffer(loadorder);

			if (loadorder != vector<int>{0, 0, 0, 0}) {
				if (load.wait_until(load_unlock, std::chrono::system_clock::now() + std::chrono::microseconds(3000),
					[&]() {;
				//check if partial load order will fit in buffer
				if (part_order(loadorder) == true) {
					return true;
				}
				else {
					return false;
				}
					}
				)) {
					//console output

					/*cout << endl;
					cout << "---------------------------------------" << endl;
					cout << "Entered in predicate true condition" << endl;
					cout << "---------------------------------------" << endl;
					cout << "Current Time : " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - globalTime).count() << " us" << endl;
					cout << "Part_Worker ID : " << i << endl;
					cout << "Iteration : " << PartWorkerIteration << endl;
					cout << "Status : WakeUp-Notified " << endl;
					cout << "Accumulated Wait Time : " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - accStartTime).count() << " us" << endl;
					cout << "Buffer State : " << "( " << sharedBuffer[0] << "," << sharedBuffer[1] << "," << sharedBuffer[2] << "," << sharedBuffer[3] << " )" << endl;
					cout << "Load Order : " << "[ " << loCopy[0] << "," << loCopy[1] << "," << loCopy[2] << "," << loCopy[3] << " ]" << endl;*/

					//writing to text file
					mout << endl;
					mout << "---------------------------------------" << endl;
					mout << "Entered in predicate true condition" << endl;
					mout << "---------------------------------------" << endl;
					mout << "Current Time : " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - globalTime).count() << " us" << endl;
					mout << "Part_Worker ID : " << i << endl;
					mout << "Iteration : " << PartWorkerIteration << endl;
					mout << "Status : WakeUp-Notified " << endl;
					mout << "Accumulated Wait Time : " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - accStartTime).count() << " us" << endl;
					mout << "Buffer State : " << "( " << sharedBuffer[0] << "," << sharedBuffer[1] << "," << sharedBuffer[2] << "," << sharedBuffer[3] << " )" << endl;
					mout << "Load Order : " << "[ " << loadorder[0] << "," << loadorder[1] << "," << loadorder[2] << "," << loadorder[3] << " ]" << endl;

					load_buffer(loadorder);
				}
				else {

					//console output
					/*cout << "---------------------------------------" << endl;
					cout << "Entered in predicate false condition" << endl;
					cout << "---------------------------------------" << endl;
					cout << "Current Time : " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - globalTime).count() << " us" << endl;
					cout << "Part_Worker ID : " << i << endl;
					cout << "Iteration : " << PartWorkerIteration << endl;
					cout << "Status : WakeUp-Timedout " << endl;
					cout << "Accumulated Wait Time : " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - accStartTime).count() << " us" << endl;
					cout << "Buffer State : " << "( " << sharedBuffer[0] << "," << sharedBuffer[1] << "," << sharedBuffer[2] << "," << sharedBuffer[3] << " )" << endl;
					cout << "Load Order : " << "[ " << loadorder[0] << "," << loadorder[1] << "," << loadorder[2] << "," << loadorder[3] << " ]" << endl;
					cout << "This load order will be destroyed !" << endl;*/

					timed_out = true;

					//file output
					mout << endl;
					mout << "---------------------------------------" << endl;
					mout << "Entered in predicate false condition" << endl;
					mout << "---------------------------------------" << endl;
					mout << "Current Time : " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - globalTime).count() << " us" << endl;
					mout << "Part_Worker ID : " << i << endl;
					mout << "Iteration : " << PartWorkerIteration << endl;
					mout << "Status : WakeUp-Timedout " << endl;
					mout << "Accumulated Wait Time : " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - accStartTime).count() << " us" << endl;
					mout << "Buffer State : " << "( " << sharedBuffer[0] << "," << sharedBuffer[1] << "," << sharedBuffer[2] << "," << sharedBuffer[3] << " )" << endl;
					mout << "Load Order : " << "[ " << loadorder[0] << "," << loadorder[1] << "," << loadorder[2] << "," << loadorder[3] << " ]" << endl;
					mout << "Updated Buffer State : " << "( " << sharedBuffer[0] << "," << sharedBuffer[1] << "," << sharedBuffer[2] << "," << sharedBuffer[3] << " )" << endl;
					mout << "Load Order to be discarded : " << "[ " << loadorder[0] << "," << loadorder[1] << "," << loadorder[2] << "," << loadorder[3] << " ]" << endl;

				}
			}
			unload.notify_all();
		} // ending of unique lock scope
	}

	//function to load parts in buffer
	void load_buffer(vector<int>& loadorder) {

		vector<int> tempOrder = loadorder;

		if (loadorder != vector<int>{0, 0, 0, 0}) {
			//loading part A
			if (sharedBuffer[0] < 6) {
				if (sharedBuffer[0] + loadorder[0] <= 6) {
					sharedBuffer[0] = sharedBuffer[0] + loadorder[0];
					loadorder[0] = 0;
				}
				else {
					loadorder[0] = (loadorder[0] + sharedBuffer[0]) - 6;
					sharedBuffer[0] = 6; //shared buffer part A is full now

				}
			}
			//loding part B
			if (sharedBuffer[1] < 5) {

				if (sharedBuffer[1] + loadorder[1] <= 5) {
					sharedBuffer[1] = sharedBuffer[1] + loadorder[1];
					loadorder[1] = 0;
				}
				else {
					loadorder[1] = (loadorder[1] + sharedBuffer[1]) - 5;
					sharedBuffer[1] = 5; // sharedbuffer part B is full now

				}
			}
			//loading part C
			if (sharedBuffer[2] < 4) {

				if (sharedBuffer[2] + loadorder[2] <= 4) {
					sharedBuffer[2] = sharedBuffer[2] + loadorder[2];
					loadorder[2] = 0;
				}
				else {
					loadorder[2] = (loadorder[2] + sharedBuffer[2]) - 4;
					sharedBuffer[2] = 4; // sharedBuffer C is full
				}
			}
			//loading part D
			if (sharedBuffer[3] < 3) {

				if (sharedBuffer[3] + loadorder[3] <= 3) {
					sharedBuffer[3] = sharedBuffer[3] + loadorder[3];
					loadorder[3] = 0;
				}
				else {
					loadorder[3] = (loadorder[3] + sharedBuffer[3]) - 3;
					sharedBuffer[3] = 3;
				}
			}

			//console output
			/*cout << "Updated Buffer State : " << "( " << sharedBuffer[0] << "," << sharedBuffer[1] << "," << sharedBuffer[2] << "," << sharedBuffer[3] << " )" << endl;
			cout << "Updated Load Order : " << "[ " << loadorder[0] << "," << loadorder[1] << "," << loadorder[2] << "," << loadorder[3] << " ]" << endl;*/

			//write to text file
			mout << "Updated Buffer State : " << "( " << sharedBuffer[0] << "," << sharedBuffer[1] << "," << sharedBuffer[2] << "," << sharedBuffer[3] << " )" << endl;
			mout << "Updated Load Order : " << "[ " << loadorder[0] << "," << loadorder[1] << "," << loadorder[2] << "," << loadorder[3] << " ]" << endl;

		}
	}

	//fnction to verify if load order can be loaded in the buffer
	bool part_order(vector<int>& lo) {

		if (sharedBuffer[0] + lo[0] <= 6 && sharedBuffer[1] + lo[1] <= 5 && sharedBuffer[2] + lo[2] <= 4 && sharedBuffer[3] + lo[3] <= 3) {
			return true;
		}
		else {
			return false;
		}
	}

	//function to get the lock and collect parts from buffer
	void collectOrder(vector<int> pickupOrder, int i, int productWorkerIteration, bool& timedOut, vector<int>& poToBeDiscarded, vector<int>& poTemp) {


		{
			unique_lock<mutex> pickup_unlock(access);
			auto accStartTime = std::chrono::steady_clock::now();

			//if buffer is compltely 0 and worker comes for pickup they will sleep and wait to be notified
			while (sharedBuffer == vector<int>{0, 0, 0, 0}) {
				unload.wait(pickup_unlock);
			}

			//console output 

			//cout << endl;
			//cout << "Current Time : " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - globalTime).count() << " us" << endl;
			//cout << "Product worker ID: " << i << endl;
			//cout << "Iteration: " << productWorkerIteration << endl;
			//cout << "Status : New PickUp Order " << endl;
			//cout << "Accumulated Wait Time : " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - accStartTime).count() << " us" << endl;
			//cout << "Buffer State : " << "( " << sharedBuffer[0] << "," << sharedBuffer[1] << "," << sharedBuffer[2] << "," << sharedBuffer[3] << " )" << endl;
			//cout << "Pickup Order : " << "[" << pickupOrder[0] << "," << pickupOrder[1] << "," << pickupOrder[2] << "," << pickupOrder[3] << " ]" << endl;


			//write to file
			mout << endl;
			mout << "Current Time : " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - globalTime).count() << " us" << endl;
			mout << "Product worker ID: " << i << endl;
			mout << "Iteration: " << productWorkerIteration << endl;
			mout << "Status : New PickUp Order " << endl;
			mout << "Accumulated Wait Time : " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - accStartTime).count() << " us" << endl;
			mout << "Buffer State : " << "( " << sharedBuffer[0] << "," << sharedBuffer[1] << "," << sharedBuffer[2] << "," << sharedBuffer[3] << " )" << endl;
			mout << "Pickup Order : " << "[" << pickupOrder[0] << "," << pickupOrder[1] << "," << pickupOrder[2] << "," << pickupOrder[3] << " ]" << endl;

			//remove parts rom buffer
			remove_buffer(pickupOrder);

			if (pickupOrder != vector<int>{0, 0, 0, 0}) {
				//woker will wait near the buffer as it has not complted pickup order 

				if (unload.wait_until(pickup_unlock, std::chrono::system_clock::now() + std::chrono::microseconds(6000),
					[&]() {
						if (product_order(pickupOrder) == true) {
							return true;
						}
						else {
							return false;
						}
					})) {

					//console output

					/*cout << endl;
					cout << "---------------------------------------" << endl;
					cout << "Entered in predicate True condition" << endl;
					cout << "---------------------------------------" << endl;
					cout << "Current Time : " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - globalTime).count() << " us" << endl;
					cout << "Product worker ID: " << i << endl;
					cout << "Iteration: " << productWorkerIteration << endl;
					cout << "Status : WakeUp-Notified " << endl;
					cout << "Buffer State : " << "( " << sharedBuffer[0] << "," << sharedBuffer[1] << "," << sharedBuffer[2] << "," << sharedBuffer[3] << " )" << endl;
					cout << "Pickup Order : " << "[" << pickupOrder[0] << "," << pickupOrder[1] << "," << pickupOrder[2] << "," << pickupOrder[3] << " ]" << endl;*/


					//write in text file
					mout << endl;
					mout << "---------------------------------------" << endl;
					mout << "Entered in predicate True condition" << endl;
					mout << "---------------------------------------" << endl;
					mout << "Current Time : " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - globalTime).count() << " us" << endl;
					mout << "Product worker ID: " << i << endl;
					mout << "Iteration: " << productWorkerIteration << endl;
					mout << "Status : WakeUp-Notified " << endl;
					mout << "Buffer State : " << "( " << sharedBuffer[0] << "," << sharedBuffer[1] << "," << sharedBuffer[2] << "," << sharedBuffer[3] << " )" << endl;
					mout << "Pickup Order : " << "[" << pickupOrder[0] << "," << pickupOrder[1] << "," << pickupOrder[2] << "," << pickupOrder[3] << " ]" << endl;

					//remove parts rom buffer
					remove_buffer(pickupOrder);

				}
				else {

					//calculate the parts you have partially picked up as you'll be discarding number of parts you partially collected
					poToBeDiscarded = { (poTemp[0] - pickupOrder[0]) , (poTemp[1] - pickupOrder[1]) ,(poTemp[2] - pickupOrder[2]) ,(poTemp[3] - pickupOrder[3]) };

					//console output

					//cout << "---------------------------------------" << endl;
					//cout << "Entered in predicate false condition" << endl;
					//cout << "---------------------------------------" << endl;
					//cout << "Current Time : " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - globalTime).count() << " us" << endl;
					//cout << "Product worker ID: " << i << endl;
					//cout << "Iteration: " << productWorkerIteration << endl;
					//cout << "Status : WakeUp-Notified " << endl;
					//cout << "Buffer State : " << "( " << sharedBuffer[0] << "," << sharedBuffer[1] << "," << sharedBuffer[2] << "," << sharedBuffer[3] << " )" << endl;
					//cout << "Pickup Order : " << "[" << pickupOrder[0] << "," << pickupOrder[1] << "," << pickupOrder[2] << "," << pickupOrder[3] << " ]" << endl;
					//cout << "Updated Buffer State : " << "( " << sharedBuffer[0] << "," << sharedBuffer[1] << "," << sharedBuffer[2] << "," << sharedBuffer[3] << " )" << endl;
					//cout << "Pickup Order to be destroyed : " << "[" << poToBeDiscarded[0] << "," << poToBeDiscarded[1] << "," << poToBeDiscarded[2] << "," << poToBeDiscarded[3] << " ]" << endl;


					// write to text file
					mout << endl;
					mout << "---------------------------------------" << endl;
					mout << "Entered in predicate false condition" << endl;
					mout << "---------------------------------------" << endl;
					mout << "Current Time : " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - globalTime).count() << " us" << endl;
					mout << "Product worker ID: " << i << endl;
					mout << "Iteration: " << productWorkerIteration << endl;
					mout << "Status : WakeUp-Notified " << endl;
					mout << "Buffer State : " << "( " << sharedBuffer[0] << "," << sharedBuffer[1] << "," << sharedBuffer[2] << "," << sharedBuffer[3] << " )" << endl;
					mout << "Pickup Order : " << "[" << pickupOrder[0] << "," << pickupOrder[1] << "," << pickupOrder[2] << "," << pickupOrder[3] << " ]" << endl;
					mout << "Updated Buffer State : " << "( " << sharedBuffer[0] << "," << sharedBuffer[1] << "," << sharedBuffer[2] << "," << sharedBuffer[3] << " )" << endl;
					mout << "Pickup Order to be destroyed : " << "[" << poToBeDiscarded[0] << "," << poToBeDiscarded[1] << "," << poToBeDiscarded[2] << "," << poToBeDiscarded[3] << " ]" << endl;

					timedOut = true;

				}
			}
			load.notify_all();
		}//end of lock
	}

	// function to remove parts from buffer
	void remove_buffer(vector<int>& pickupOrder) {

		vector<int> temporder = pickupOrder;

		if (sharedBuffer != vector<int>{0, 0, 0, 0}) {

			//pickingup part A from buffer
			if (sharedBuffer[0] >= pickupOrder[0]) {
				sharedBuffer[0] = sharedBuffer[0] - pickupOrder[0];
				pickupOrder[0] = 0;
			}
			else if (sharedBuffer[0] < pickupOrder[0]) {
				pickupOrder[0] = pickupOrder[0] - sharedBuffer[0];
				sharedBuffer[0] = 0;
			}

			//pickingup part B from buffer
			if (sharedBuffer[1] >= pickupOrder[1]) {
				sharedBuffer[1] = sharedBuffer[1] - pickupOrder[1];
				pickupOrder[1] = 0;
			}
			else if (pickupOrder[1] > sharedBuffer[1]) {
				pickupOrder[1] = pickupOrder[1] - sharedBuffer[1];
				sharedBuffer[1] = 0;
			}

			//pickingup part C from buffer

			if (sharedBuffer[2] >= pickupOrder[2]) {
				sharedBuffer[2] = sharedBuffer[2] - pickupOrder[2];
				pickupOrder[2] = 0;
			}
			else if (pickupOrder[2] > sharedBuffer[2]) {
				pickupOrder[2] = pickupOrder[2] - sharedBuffer[2];
				sharedBuffer[2] = 0;
			}

			//pickong up part D from buffer
			if (sharedBuffer[3] >= pickupOrder[3]) {
				sharedBuffer[3] = sharedBuffer[3] - pickupOrder[3];
				pickupOrder[3] = 0;
			}
			else if (pickupOrder[3] > sharedBuffer[3]) {
				pickupOrder[3] = pickupOrder[3] - sharedBuffer[3];
				sharedBuffer[3] = 0;
			}
		}
		//console output

		//cout << "Updated Buffer : " << "( " << sharedBuffer[0] << "," << sharedBuffer[1] << "," << sharedBuffer[2] << "," << sharedBuffer[3] << " )" << endl;
		//cout << "Updated Pickup Order : " << "[" << pickupOrder[0] << "," << pickupOrder[1] << "," << pickupOrder[2] << "," << pickupOrder[3] << " ]" << endl;


		mout << "Updated Buffer : " << "( " << sharedBuffer[0] << "," << sharedBuffer[1] << "," << sharedBuffer[2] << "," << sharedBuffer[3] << " )" << endl;
		mout << "Updated Pickup Order : " << "[" << pickupOrder[0] << "," << pickupOrder[1] << "," << pickupOrder[2] << "," << pickupOrder[3] << " ]" << endl;

		if (pickupOrder == vector<int>{0, 0, 0, 0}) {
			numOfProducts++;
			//cout << "Total Completed Products : " << numOfProducts << endl;
			mout << "Total Completed Products : " << numOfProducts << endl;
		}
	}

	//function to verify is pickup order can be completed 
	bool product_order(vector<int>& po) {

		if (po[0] < sharedBuffer[0] && po[1] < sharedBuffer[1] && po[2] < sharedBuffer[2] && po[3] < sharedBuffer[3]) {
			return true;
		}
		else {
			return false;
		}
	}
};

/*
  **************************************************************************
  ************** Functions to be excuted without lock **********************
  **************************************************************************
*/

//function to generate random number
int getRandom() {
	int n;
	srand(seed);
	n = rand() % 50;
	seed++;
	return n;
}

//function to genrate table of all legal load order
void load_order(vector<vector<int>>& pl) {
	//cout << "load order will be generated here" << endl;

	vector<int> legalProductOrder{ 0,0,0,0 };
	int k = 0;
	int entries = 0;
	while (1) {
		int sum = 0;
		// obtain a random number from hardware
		std::random_device rd;
		// seed the generator
		std::mt19937 eng(rd());
		// define the range
		std::uniform_int_distribution<> distr(0, 4);

		for (int x = 0; x < 4; x++) {

			legalProductOrder[x] = distr(eng);
			sum = sum + legalProductOrder[x];

		}

		if (sum == 4) {
			if (legalProductOrder[0] == 0 && legalProductOrder[1] == 0 && legalProductOrder[2] == 0 && legalProductOrder[3] > 0 ||
				legalProductOrder[0] == 0 && legalProductOrder[1] == 0 && legalProductOrder[2] > 0 && legalProductOrder[3] == 0 ||
				legalProductOrder[0] == 0 && legalProductOrder[1] > 0 && legalProductOrder[2] == 0 && legalProductOrder[3] == 0 ||
				legalProductOrder[0] > 0 && legalProductOrder[1] == 0 && legalProductOrder[2] == 0 && legalProductOrder[3] == 0) {

				continue;

			}
			else {
				for (int x = 0; x < legalProductOrder.size(); x++) {
					pl[entries][x] = legalProductOrder[x];
				}
				entries++;
				k++;
			}
		}

		if (k == 50) {
			break;
		}
	}
}

//function to generate all the product order
void product_order(vector<vector<int>>& po) {
	//cout << "product order will be generated here" << endl;

	vector<int> legalProductOrder{ 0,0,0,0 };
	int k = 0;
	int entries = 0;
	while (1) {
		int sum = 0;
		// obtain a random number from hardware
		std::random_device rd;
		// seed the generator
		std::mt19937 eng(rd());
		// define the range
		std::uniform_int_distribution<> distr(0, 4);

		for (int x = 0; x < 4; x++) {

			legalProductOrder[x] = distr(eng);
			sum = sum + legalProductOrder[x];

		}

		if (sum == 5) {
			if (legalProductOrder[0] == 0 && legalProductOrder[1] == 0 && legalProductOrder[2] == 0 && legalProductOrder[3] == 0 ||
				legalProductOrder[0] > 0 && legalProductOrder[1] == 0 && legalProductOrder[2] == 0 && legalProductOrder[3] == 0 ||
				legalProductOrder[0] == 0 && legalProductOrder[1] > 0 && legalProductOrder[2] == 0 && legalProductOrder[3] == 0 ||
				legalProductOrder[0] == 0 && legalProductOrder[1] == 0 && legalProductOrder[2] > 0 && legalProductOrder[3] == 0 ||
				legalProductOrder[0] == 0 && legalProductOrder[1] == 0 && legalProductOrder[2] == 0 && legalProductOrder[3] > 0 ||
				legalProductOrder[0] > 0 && legalProductOrder[1] > 0 && legalProductOrder[2] == 0 && legalProductOrder[3] == 0 ||
				legalProductOrder[0] == 0 && legalProductOrder[1] > 0 && legalProductOrder[2] > 0 && legalProductOrder[3] == 0 ||
				legalProductOrder[0] == 0 && legalProductOrder[1] == 0 && legalProductOrder[2] > 0 && legalProductOrder[3] > 0 ||
				legalProductOrder[0] > 0 && legalProductOrder[1] == 0 && legalProductOrder[2] == 0 && legalProductOrder[3] > 0 ||
				legalProductOrder[0] > 0 && legalProductOrder[1] == 0 && legalProductOrder[2] > 0 && legalProductOrder[3] == 0 ||
				legalProductOrder[0] == 0 && legalProductOrder[1] > 0 && legalProductOrder[2] == 0 && legalProductOrder[3] > 0 ||
				legalProductOrder[0] > 0 && legalProductOrder[1] > 0 && legalProductOrder[2] > 0 && legalProductOrder[3] > 0) {

				continue;

			}
			else {
				for (int x = 0; x < legalProductOrder.size(); x++) {
					po[entries][x] = legalProductOrder[x];
				}
				entries++;
				k++;
			}
		}

		if (k == 50) {
			break;
		}
	}
}

//function to excute the time it takes for part worker to produce and move parts 
void part_workerLag(vector<int>& lOrder) {
	int partProductionTime = ((lOrder[0] * 50) + (lOrder[1] * 70) + (lOrder[2] * 90) + (lOrder[3] * 90));
	int partMoveingTime = ((lOrder[0] * 20) + (lOrder[1] * 30) + (lOrder[2] * 40) + (lOrder[3] * 50));

	int wait_time = partProductionTime + partMoveingTime;

	//sleeping before aquiring lock
	this_thread::sleep_for(std::chrono::microseconds(wait_time));
}

//function to execute the time it takes for product worker to move and assemble parts
void product_workerLag(vector<int>& pOrder) {

	int moveToAssembly = ((pOrder[0] * 20) + (pOrder[1] * 30) + (pOrder[2] * 40) + (pOrder[3] * 50));
	int assemblyTime = ((pOrder[0] * 80) + (pOrder[1] * 100) + (pOrder[2] * 120) + (pOrder[3] * 140));

	int totalLagTime = moveToAssembly + assemblyTime;

	//sleeping after leaving the lock
	this_thread::sleep_for(std::chrono::microseconds(totalLagTime));
}

// function to execute discarding of load order
void part_discard(vector<int>& lo) {
	int discardT = ((lo[0] * 20) + (lo[1] * 30) + (lo[2] * 40) + (lo[3] * 50));
	lo = vector<int>{ 0,0,0,0 };

	//sleep while it discard the remaing order which it could not put in buffer
	this_thread::sleep_for(std::chrono::microseconds(discardT));
}

//function to execute discarding of pickup order
void product_discard(vector<int>& po) {
	int discardProductT = ((po[0] * 20) + (po[1] * 30) + (po[2] * 40) + (po[3] * 50));
	po = vector<int>{ 0,0,0,0 };

	//sleep for the time it will discard the partial pick up order it took from the buffer
	this_thread::sleep_for(std::chrono::microseconds(discardProductT));
}

//function to execute part worker thread
void partWorker(int i, blocking_queue& centralManager) {

	int PartWorkerIteration = 0;

	while (PartWorkerIteration < 6) {

		int z = getRandom();
		vector<int> loadorder{ 0,0,0,0 };

		loadorder = loadOrderList[z];

		part_workerLag(loadorder);
		bool timed_out = false;

		//take the lock and add the order to buffer
		centralManager.putIn(loadorder, i, PartWorkerIteration, timed_out);

		//load order is discarded after leaving lock
		if (timed_out == true) {
			part_discard(loadorder);
		}

		PartWorkerIteration++;
	}
}

//function to execute product worker thread
void productWorker(int i, blocking_queue& centralManager) {

	int productWorkerIteration = 0;
	while (productWorkerIteration < 6) {


		int z = getRandom();
		vector<int> pickupOrder{ 0,0,0,0 };
		pickupOrder = pickupOrderList[z];
		vector<int> poTemp = pickupOrderList[z];
		vector<int> poToBeDiscarded{ 0,0,0,0 };
		bool timedOut = false;

		//get the lock and coolect the order
		centralManager.collectOrder(pickupOrder, i, productWorkerIteration, timedOut, poToBeDiscarded, poTemp);

		if (timedOut == false) {
			product_workerLag(poTemp);
		}
		else {
			product_discard(poToBeDiscarded);
		}

		productWorkerIteration++;
	}
}


int main() {

	blocking_queue centralManager;

	mout << "-----------------------------------------" << endl;
	mout << "|            Dhruv G Bhatti             |" << endl;
	mout << "-----------------------------------------" << endl;
	mout << endl;
	mout << "Lets start the production of parts!! " << endl;
	mout << endl;

	cout << "output is being written to text file..." << endl;

	globalTime = std::chrono::steady_clock::now();

	load_order(loadOrderList);
	product_order(pickupOrderList);

	const int m = 20;
	const int n = 16;

	thread partWorkerThread[m];
	thread productWorkerThread[n];
	for (int i = 0; i < n; i++) {
		partWorkerThread[i] = thread(partWorker, i, ref(centralManager));
		productWorkerThread[i] = thread(productWorker, i, ref(centralManager));
	}
	for (int i = n; i < m; i++) {
		partWorkerThread[i] = thread(partWorker, i, ref(centralManager));
	}

	for (int i = 0; i < n; i++) {
		partWorkerThread[i].join();
		productWorkerThread[i].join();
	}
	for (int i = n; i < m; i++) {
		partWorkerThread[i].join();
	}

	//console output
	/*
	cout << " " << endl;
	cout << " Production Complete !! " << endl;
	cout << "total products " << numOfProducts << endl;
	*/

	mout << endl;
	mout << " Hurraaayyyy !!!!  Production Completed !! " << endl;
	mout << "total products we developed today: " << numOfProducts << endl;

	mout.close();
	cout << "Here!!! All set. Report is generated and text file is ready. " << endl;
	getchar();
	getchar();
	return 0;
}
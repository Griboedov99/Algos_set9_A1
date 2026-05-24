#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <fstream>
#include <chrono>

using namespace std;

long long charComparisons = 0;

// =====================================================
// Подсчет посимвольных сравнений
// =====================================================

bool stringLess(const string& a, const string& b) {
    int n = min(a.size(), b.size());

    for (int i = 0; i < n; ++i) {
        charComparisons++;

        if (a[i] < b[i]) {
            return true;
        }

        if (a[i] > b[i]) {
            return false;
        }
    }

    charComparisons++;

    return a.size() < b.size();
}

// =====================================================
// QUICK SORT
// =====================================================

int partition(vector<string>& arr, int low, int high) {
    string pivot = arr[high];

    int i = low - 1;

    for (int j = low; j < high; ++j) {

        if (stringLess(arr[j], pivot)) {
            ++i;
            swap(arr[i], arr[j]);
        }
    }

    swap(arr[i + 1], arr[high]);

    return i + 1;
}

void quickSort(vector<string>& arr, int low, int high) {

    if (low < high) {

        int pi = partition(arr, low, high);

        quickSort(arr, low, pi - 1);

        quickSort(arr, pi + 1, high);
    }
}

// =====================================================
// MERGE SORT
// =====================================================

void merge(
    vector<string>& arr,
    int left,
    int mid,
    int right
) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    vector<string> L(n1);
    vector<string> R(n2);

    for (int i = 0; i < n1; ++i) {
        L[i] = arr[left + i];
    }

    for (int i = 0; i < n2; ++i) {
        R[i] = arr[mid + 1 + i];
    }

    int i = 0;
    int j = 0;
    int k = left;

    while (i < n1 && j < n2) {

        if (!stringLess(R[j], L[i])) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
    }

    while (i < n1) {
        arr[k++] = L[i++];
    }

    while (j < n2) {
        arr[k++] = R[j++];
    }
}

void mergeSort(vector<string>& arr, int left, int right) {

    if (left < right) {

        int mid = left + (right - left) / 2;

        mergeSort(arr, left, mid);

        mergeSort(arr, mid + 1, right);

        merge(arr, left, mid, right);
    }
}

// =====================================================
// STRING GENERATOR
// =====================================================

class StringGenerator {
private:
    const string alphabet =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789"
        "!@#%:;^&*()-.";

    mt19937 rng;

public:
    StringGenerator() {
        rng.seed(random_device{}());
    }

    string generateString(int minLen = 10, int maxLen = 200) {

        uniform_int_distribution<int> lenDist(minLen, maxLen);

        int len = lenDist(rng);

        uniform_int_distribution<int> charDist(
            0,
            alphabet.size() - 1
        );

        string s;

        for (int i = 0; i < len; ++i) {
            s += alphabet[charDist(rng)];
        }

        return s;
    }

    vector<string> generateRandomArray(int size) {

        vector<string> arr(size);

        for (int i = 0; i < size; ++i) {
            arr[i] = generateString();
        }

        return arr;
    }
};

// =====================================================
// MAIN
// =====================================================

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    ofstream fout("benchmark.csv");

    fout << "algorithm,array_type,array_size,time_us,char_comparisons\n";

    StringGenerator generator;

    // Базовый случайный массив
    vector<string> randomBase =
        generator.generateRandomArray(3000);

    // Обратно отсортированный
    vector<string> reversedBase = randomBase;

    sort(reversedBase.begin(), reversedBase.end());

    reverse(reversedBase.begin(), reversedBase.end());

    // Почти отсортированный
    vector<string> almostSortedBase = randomBase;

    sort(almostSortedBase.begin(), almostSortedBase.end());

    for (int i = 0; i < 30; ++i) {

        swap(
            almostSortedBase[rand() % 3000],
            almostSortedBase[rand() % 3000]
        );
    }

    // Тестирование
    for (int size = 100; size <= 3000; size += 100) {

        vector<pair<string, vector<string>>> tests = {

            {
                "random",
                vector<string>(
                    randomBase.begin(),
                    randomBase.begin() + size
                )
            },

            {
                "reversed",
                vector<string>(
                    reversedBase.begin(),
                    reversedBase.begin() + size
                )
            },

            {
                "almost_sorted",
                vector<string>(
                    almostSortedBase.begin(),
                    almostSortedBase.begin() + size
                )
            }
        };

        for (auto& test : tests) {

            string arrayType = test.first;

            vector<string> arr = test.second;

            // =========================================
            // QUICK SORT
            // =========================================

            vector<string> quickArr = arr;

            charComparisons = 0;

            auto startQuick =
                chrono::high_resolution_clock::now();

            quickSort(
                quickArr,
                0,
                quickArr.size() - 1
            );

            auto endQuick =
                chrono::high_resolution_clock::now();

            auto quickTime =
                chrono::duration_cast<
                    chrono::microseconds
                >(endQuick - startQuick).count();

            fout << "QuickSort,"
                 << arrayType << ","
                 << size << ","
                 << quickTime << ","
                 << charComparisons << "\n";

            // =========================================
            // MERGE SORT
            // =========================================

            vector<string> mergeArr = arr;

            charComparisons = 0;

            auto startMerge =
                chrono::high_resolution_clock::now();

            mergeSort(
                mergeArr,
                0,
                mergeArr.size() - 1
            );

            auto endMerge =
                chrono::high_resolution_clock::now();

            auto mergeTime =
                chrono::duration_cast<
                    chrono::microseconds
                >(endMerge - startMerge).count();

            fout << "MergeSort,"
                 << arrayType << ","
                 << size << ","
                 << mergeTime << ","
                 << charComparisons << "\n";

            cout << "Processed: "
                 << arrayType
                 << ", size = "
                 << size
                 << endl;
        }
    }

    fout.close();

    cout << "\nBenchmark completed.\n";

    return 0;
}

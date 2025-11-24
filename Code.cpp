#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <iomanip>
#include <cctype>
#include <cmath>
#include <limits>
#include <sstream>

using namespace std;

const int MIN_TRACK = 0;
const int MAX_TRACK = 199;

// convert whole string to uppercase
string toUpper(string s) {
    for (char &c : s) {
        c = static_cast<char>(toupper(static_cast<unsigned char>(c)));
    }
    return s;
}

// clear bad numeric input
void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// ignore rest of current line
void ignoreLine() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main() {
    char again = 'Y';

    cout << "=== C-SCAN Disk Scheduling Simulator (Tracks 0 - 199) ===\n\n";

    while (toupper(static_cast<unsigned char>(again)) == 'Y') {
        int initialPos;
        string direction;
        int n;

        // 1. initial arm position
        while (true) {
            cout << "Enter initial arm position (0 - 199): ";
            cin >> initialPos;

            if (cin.fail()) {
                cout << "Invalid input. Please enter an integer.\n";
                clearInput();
                continue;
            }
            if (initialPos < MIN_TRACK || initialPos > MAX_TRACK) {
                cout << "Error: Initial arm position must be between 0 and 199.\n";
                continue;
            }
            break;
        }

        // 2. direction
        while (true) {
            cout << "Enter direction (UP / DOWN): ";
            cin >> direction;
            direction = toUpper(direction);

            if (direction == "UP" || direction == "DOWN")
                break;

            cout << "Error: Direction must be UP or DOWN.\n";
        }

        // 3. number of requests
        while (true) {
            cout << "Enter number of track requests: ";
            cin >> n;

            if (cin.fail()) {
                cout << "Invalid input. Please enter a positive integer.\n";
                clearInput();
                continue;
            }
            if (n <= 0) {
                cout << "Error: Number of requests must be positive.\n";
                continue;
            }
            break;
        }

        // 4. read requests
        vector<int> requests;
        requests.reserve(n);

        cout << "Enter " << n << " track requests (0 - 199):\n";

        // read one whole line of numbers first
        ignoreLine();        // remove leftover '\n' after n
        string line;
        getline(cin, line);

        if (!line.empty()) {
            istringstream iss(line);
            int value;
            while (iss >> value) {
                if (value < MIN_TRACK || value > MAX_TRACK) {
                    cout << "Warning: " << value
                         << " is out of range (ignored, please re-enter later).\n";
                    continue;
                }
                requests.push_back(value);
                if ((int)requests.size() == n) break;
            }
        }

        // if not enough numbers, ask for the remaining as Request x
        while ((int)requests.size() < n) {
            int idx = (int)requests.size(); // 0-based
            int value;
            cout << "Request " << (idx + 1) << ": ";
            cin >> value;

            if (cin.fail()) {
                cout << "Invalid input. Please enter an integer between 0 and 199.\n";
                clearInput();
                continue;
            }
            if (value < MIN_TRACK || value > MAX_TRACK) {
                cout << "Error: Track request must be between 0 and 199.\n";
                continue;
            }
            requests.push_back(value);
        }

        // 5. sort
        sort(requests.begin(), requests.end());

        cout << "\nSorted requests: ";
        for (int r : requests) cout << r << " ";
        cout << "\n";

        // 6. C-SCAN scheduling
        int head = initialPos;
        long long totalMovement = 0;

        struct Step {
            int start;
            int finish;
            int distance;
        };
        vector<Step> steps;

        if (direction == "UP") {
            vector<int> up, down;
            for (int r : requests) {
                if (r >= head) up.push_back(r);
                else down.push_back(r);
            }

            int current = head;

            // up side first
            for (int r : up) {
                int dist = abs(r - current);
                steps.push_back({ current, r, dist });
                totalMovement += dist;
                current = r;
            }

            // go to 199
            if (current != MAX_TRACK) {
                int dist = abs(MAX_TRACK - current);
                steps.push_back({ current, MAX_TRACK, dist });
                totalMovement += dist;
                current = MAX_TRACK;
            }

            // jump 199 -> 0 and do lower side
            if (!down.empty()) {
                int dist = abs(MAX_TRACK - MIN_TRACK);
                steps.push_back({ MAX_TRACK, MIN_TRACK, dist });
                totalMovement += dist;
                current = MIN_TRACK;

                for (int r : down) {
                    int dist2 = abs(r - current);
                    steps.push_back({ current, r, dist2 });
                    totalMovement += dist2;
                    current = r;
                }
            }

        } else { // DOWN
            vector<int> down, up;
            for (int r : requests) {
                if (r <= head) down.push_back(r);
                else up.push_back(r);
            }

            sort(down.begin(), down.end(), greater<int>());
            sort(up.begin(), up.end(), greater<int>());

            int current = head;

            // down side first
            for (int r : down) {
                int dist = abs(r - current);
                steps.push_back({ current, r, dist });
                totalMovement += dist;
                current = r;
            }

            // go to 0
            if (current != MIN_TRACK) {
                int dist = abs(current - MIN_TRACK);
                steps.push_back({ current, MIN_TRACK, dist });
                totalMovement += dist;
                current = MIN_TRACK;
            }

            // jump 0 -> 199 and do upper side
            if (!up.empty()) {
                int dist = abs(MAX_TRACK - MIN_TRACK);
                steps.push_back({ MIN_TRACK, MAX_TRACK, dist });
                totalMovement += dist;
                current = MAX_TRACK;

                for (int r : up) {
                    int dist2 = abs(current - r);
                    steps.push_back({ current, r, dist2 });
                    totalMovement += dist2;
                    current = r;
                }
            }
        }

        // 7. output
        cout << "\n=== C-SCAN Schedule ===\n";
        cout << "Initial Arm Position: " << initialPos << "\n";
        cout << "Direction: " << direction << "\n\n";

        cout << left << setw(10) << "Start"
             << left << setw(10) << "Finish"
             << left << setw(15) << "Track Travelled" << "\n";
        cout << string(35, '=') << "\n";

        for (const auto &s : steps) {
            cout << left << setw(10) << s.start
                 << left << setw(10) << s.finish
                 << left << setw(15) << s.distance << "\n";
        }

        cout << "\nTotal Track Travelled: " << totalMovement << "\n";

        // 8. repeat
        cout << "\nWould you like to run another simulation? (Y/N): ";
        cin >> again;
        cout << "\n";
    }

    cout << "Exiting program. Goodbye!\n";
    return 0;
}

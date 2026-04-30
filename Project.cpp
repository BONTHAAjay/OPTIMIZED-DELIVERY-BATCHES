#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>
#include <iomanip>
using namespace std;

struct Order {
    int id;
    string restaurant;
    string customer;
    int prepTime;
    int deadline;
    double x, y;
};

double distanceBetween(double x1, double y1, double x2, double y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

pair<vector<Order>, double> greedyRoute(vector<Order> batch) {
    vector<Order> route;
    double totalDistance = 0.0;

    double currentX = 0.0;
    double currentY = 0.0;

    while (!batch.empty()) {
        int nearestIndex = 0;
        double nearestDistance = distanceBetween(
            currentX, currentY,
            batch[0].x, batch[0].y
        );

        for (int i = 1; i < batch.size(); i++) {
            double d = distanceBetween(
                currentX, currentY,
                batch[i].x, batch[i].y
            );

            if (d < nearestDistance) {
                nearestDistance = d;
                nearestIndex = i;
            }
        }

        totalDistance += nearestDistance;
        currentX = batch[nearestIndex].x;
        currentY = batch[nearestIndex].y;

        route.push_back(batch[nearestIndex]);
        batch.erase(batch.begin() + nearestIndex);
    }

    return {route, totalDistance};
}

double batchCost(vector<Order> batch) {
    auto result = greedyRoute(batch);
    vector<Order> route = result.first;
    double totalDistance = result.second;

    int maxPrepTime = 0;
    for (auto &order : batch) {
        maxPrepTime = max(maxPrepTime, order.prepTime);
    }

    double deliveryTime = maxPrepTime + totalDistance * 5.0;
    double latePenalty = 0.0;

    double currentTime = maxPrepTime;
    double currentX = 0.0;
    double currentY = 0.0;

    for (auto &order : route) {
        double travelTime = distanceBetween(
            currentX, currentY,
            order.x, order.y
        ) * 5.0;

        currentTime += travelTime;

        if (currentTime > order.deadline) {
            latePenalty += (currentTime - order.deadline) * 10.0;
        }

        currentX = order.x;
        currentY = order.y;
    }

    return deliveryTime + latePenalty;
}

void generateBatches(
    vector<Order> &orders,
    vector<Order> &currentBatch,
    vector<vector<Order>> &allBatches,
    int start,
    int maxBatchSize
) {
    if (!currentBatch.empty()) {
        allBatches.push_back(currentBatch);
    }

    if (currentBatch.size() == maxBatchSize) {
        return;
    }

    for (int i = start; i < orders.size(); i++) {
        currentBatch.push_back(orders[i]);
        generateBatches(
            orders,
            currentBatch,
            allBatches,
            i + 1,
            maxBatchSize
        );
        currentBatch.pop_back();
    }
}

pair<vector<pair<vector<Order>, double>>, double> dynamicBatchOptimizer(
    vector<Order> orders,
    int maxBatchSize
) {
    int n = orders.size();
    int totalStates = 1 << n;

    vector<vector<Order>> allBatches;
    vector<Order> currentBatch;

    generateBatches(
        orders,
        currentBatch,
        allBatches,
        0,
        maxBatchSize
    );

    vector<int> batchMasks;
    vector<double> batchCosts;

    for (auto &batch : allBatches) {
        int mask = 0;

        for (auto &order : batch) {
            for (int i = 0; i < n; i++) {
                if (orders[i].id == order.id) {
                    mask |= (1 << i);
                    break;
                }
            }
        }

        batchMasks.push_back(mask);
        batchCosts.push_back(batchCost(batch));
    }

    vector<double> dp(totalStates, numeric_limits<double>::infinity());
    vector<int> parentMask(totalStates, -1);
    vector<int> parentBatch(totalStates, -1);

    dp[0] = 0.0;

    for (int mask = 0; mask < totalStates; mask++) {
        if (dp[mask] == numeric_limits<double>::infinity()) {
            continue;
        }

        for (int i = 0; i < allBatches.size(); i++) {
            int batchMask = batchMasks[i];

            if ((mask & batchMask) == 0) {
                int newMask = mask | batchMask;
                double newCost = dp[mask] + batchCosts[i];

                if (newCost < dp[newMask]) {
                    dp[newMask] = newCost;
                    parentMask[newMask] = mask;
                    parentBatch[newMask] = i;
                }
            }
        }
    }

    vector<pair<vector<Order>, double>> selectedBatches;

    int finalMask = totalStates - 1;

    while (finalMask != 0) {
        int batchIndex = parentBatch[finalMask];

        selectedBatches.push_back({
            allBatches[batchIndex],
            batchCosts[batchIndex]
        });

        finalMask = parentMask[finalMask];
    }

    reverse(selectedBatches.begin(), selectedBatches.end());

    return {selectedBatches, dp[totalStates - 1]};
}

int main() {
    vector<Order> orders = {
        {1, "Burger House", "Alice", 10, 35, 2, 3},
        {2, "Pizza Hub", "Bob", 15, 45, 5, 4},
        {3, "Sushi World", "Charlie", 8, 30, 1, 6},
        {4, "Taco Town", "David", 12, 50, 7, 2},
        {5, "Noodle Bar", "Emma", 7, 25, 3, 1}
    };

    cout << fixed << setprecision(2);

    cout << "AVAILABLE ORDERS\n";
    cout << "--------------------------------------------------\n";

    for (auto &order : orders) {
        cout << "Order " << order.id << ": "
             << order.restaurant << " -> "
             << order.customer
             << " | Prep: " << order.prepTime << " min"
             << " | Deadline: " << order.deadline << " min"
             << " | Location: (" << order.x << ", " << order.y << ")\n";
    }

    auto result = dynamicBatchOptimizer(orders, 3);

    vector<pair<vector<Order>, double>> selectedBatches = result.first;
    double totalCost = result.second;

    cout << "\nOPTIMIZED DELIVERY BATCHES\n";
    cout << "--------------------------------------------------\n";

    for (int i = 0; i < selectedBatches.size(); i++) {
        vector<Order> batch = selectedBatches[i].first;
        double cost = selectedBatches[i].second;

        auto routeResult = greedyRoute(batch);
        vector<Order> route = routeResult.first;
        double routeDistance = routeResult.second;

        cout << "\nBatch " << i + 1 << "\n";
        cout << "Batch Cost: " << cost << "\n";
        cout << "Delivery Route:\n";

        for (auto &order : route) {
            cout << "  Order " << order.id << ": "
                 << order.restaurant << " -> "
                 << order.customer
                 << " at (" << order.x << ", " << order.y << ")\n";
        }

        cout << "Total Route Distance: " << routeDistance << "\n";
    }

    cout << "\nFINAL RESULT\n";
    cout << "--------------------------------------------------\n";
    cout << "Minimum Total Cost: " << totalCost << "\n";

    return 0;
}
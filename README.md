# 🍔 Food Delivery Batch Optimizer (C++)

## 📌 Overview

This project simulates a **food delivery system** that efficiently groups and schedules orders using:

- ✅ **Greedy Algorithm** (for route optimization)
- ✅ **Dynamic Programming (DP)** (for batch selection)

The goal is to **minimize delivery cost** while respecting preparation times and delivery deadlines.

---

## 🚀 Problem Statement

Food delivery platforms (like Uber Eats / Zomato) need to:

- Deliver multiple orders efficiently
- Group orders into batches
- Optimize delivery routes
- Avoid late deliveries

This project solves the above using algorithmic techniques.

---

## 🧠 Algorithms Used

### 🔹 Greedy Algorithm
Used to determine the **delivery route within a batch**.

👉 Strategy:
- Always deliver to the **nearest next customer**

---

### 🔹 Dynamic Programming (Bitmask DP)
Used to **select the best combination of batches**.

👉 Strategy:
- Try all possible batches (up to size 3)
- Choose combination with **minimum total cost**

---

## ⚙️ Features

- 📦 Order dataset (restaurant → customer)
- 🚚 Batch generation (size ≤ 3)
- 📍 Route optimization using greedy
- ⏱ Deadline handling
- ⚠️ Late delivery penalty system
- 📊 Final optimized delivery plan

---

## 🏗️ Project Structure

```txt
main.cpp   → Full implementation (all logic in one file)

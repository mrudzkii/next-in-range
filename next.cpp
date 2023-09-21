#include "next.h"
#include <algorithm>
#include <climits>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <utility>
#include <vector>

using namespace std;

typedef struct bin_node *bin_tree;

vector<bin_tree> to_delete;

struct bin_node {
  int val;
  bin_tree left, right;
};

pair<int, int> main_interval;

pair<int, int> left(pair<int, int> interval) {
  return make_pair(interval.first, (interval.first + interval.second) / 2);
}

pair<int, int> right(pair<int, int> interval) {
  return make_pair((interval.first + interval.second) / 2 + 1, interval.second);
}

void generate_primary_tree(bin_tree t, pair<int, int> interval) {
  t->val = INT_MAX;
  if (interval.first != interval.second) {
    t->left = new bin_node;
    t->right = new bin_node;
    to_delete.push_back(t->left);
    to_delete.push_back(t->right);
    generate_primary_tree(t->left, left(interval));
    generate_primary_tree(t->right, right(interval));
  } else {
    t->left = NULL;
    t->left = NULL;
  }
  return;
}

void generate_query(bin_tree cur, bin_tree prev, int i, pair<int, int> interval,
                    vector<int> &v) {
  to_delete.push_back(cur);
  if (v[i] >= interval.first && v[i] <= interval.second) {
    cur->val = i;
    if (interval.first != interval.second) {
      cur->left = new bin_node;
      cur->right = new bin_node;
      generate_query(cur->left, prev->left, i, left(interval), v);
      generate_query(cur->right, prev->right, i, right(interval), v);
    }
  } else {
    cur->val = prev->val;
    cur->left = prev->left;
    cur->right = prev->right;
  }
}

deque<bin_tree> queries;
vector<int> duplicate;

void init(const vector<int> &x) {
  // I copy given vector, sort it and delete duplicates from it
  // as I still need the original to renumber the vector
  duplicate = x;
  sort(duplicate.begin(), duplicate.end());
  duplicate.erase(unique(duplicate.begin(), duplicate.end()), duplicate.end());
  vector<int> renum;
  for (auto i : x) {
    renum.push_back(int(lower_bound(duplicate.begin(), duplicate.end(), i) -
                        duplicate.begin()));
  }
  main_interval = make_pair(0, int(duplicate.size() - 1));
  bin_tree primary = new bin_node;
  generate_primary_tree(primary, main_interval);
  to_delete.push_back(primary);
  queries.push_front(primary);
  bin_tree prev = primary;
  for (int i = int(x.size() - 1); i >= 0; i--) {
    queries.push_front(new bin_node);
    generate_query(queries.front(), prev, i, main_interval, renum);
    prev = queries.front();
  }
  return;
}

int solve(bin_tree t, pair<int, int> interval, int x, int y) {
  if (interval.first > y || interval.second < x)
    return INT_MAX;
  if (interval.first >= x && interval.second <= y)
    return t->val;
  else
    return min(solve(t->left, left(interval), x, y),
               solve(t->right, right(interval), x, y));
}

int nextInRange(int i, int a, int b) {
  int res;
  a = int(lower_bound(duplicate.begin(), duplicate.end(), a) -
          duplicate.begin());
  b = int(upper_bound(duplicate.begin(), duplicate.end(), b) -
          duplicate.begin()) -
      1;
  res = solve(queries[i], main_interval, a, b);
  if (res != INT_MAX)
    return res;
  else
    return -1;
}

void done() {
  int size = int(to_delete.size());
  for (int i = 0; i < size; i++) {
    delete to_delete.back();
    to_delete.pop_back();
  }
  size = int(duplicate.size());
  for (int i = 0; i < size; i++) {
    duplicate.pop_back();
  }
  while (!queries.empty()) {
    queries.pop_front();
  }
}

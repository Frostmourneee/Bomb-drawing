#include <vector>
#include <utility>
#include <cmath>

bool ConvexHullHighRight(const std::vector<std::pair<double, double>>& inputpoints, size_t& index, std::vector<size_t>& convexhull) {
    double E = 1.e-5;
    std::vector<size_t> candidates;
    for (size_t i = 0; i < inputpoints.size(); i++) {
        if ((inputpoints[i].first >= inputpoints[index].first) && (inputpoints[i].second >= inputpoints[index].second) && (i != index)) candidates.push_back(i);
    }
    if (candidates.empty()) return false;
    double maxtan = 0;
    size_t maxtanindex = candidates[0];
    for (size_t i = 0; i < candidates.size(); i++) {
        double temptan = std::abs(inputpoints[candidates[i]].second - inputpoints[index].second) / std::abs(inputpoints[candidates[i]].first - inputpoints[index].first);
        if (temptan >= maxtan) {
            if (std::abs(temptan - maxtan) < E) {
                if (inputpoints[candidates[i]].first > inputpoints[maxtanindex].first) maxtanindex = candidates[i];
            }
            else {
                maxtan = temptan;
                maxtanindex = candidates[i];
            }
        }
    }
    convexhull.push_back(maxtanindex);
    index = maxtanindex;
    return true;
}

bool ConvexHullLowRight(const std::vector<std::pair<double, double>>& inputpoints, size_t& index, std::vector<size_t>& convexhull) {
    double E = 1.e-5;
    std::vector<size_t> candidates;
    for (size_t i = 0; i < inputpoints.size(); i++) {
        if ((inputpoints[i].first >= inputpoints[index].first) && (inputpoints[i].second <= inputpoints[index].second) && (i != index)) candidates.push_back(i);
    }
    if (candidates.empty()) return false;
    double maxctan = 0;
    size_t maxctanindex = candidates[0];
    for (size_t i = 0; i < candidates.size(); i++) {
        double tempctan = std::abs(inputpoints[candidates[i]].first - inputpoints[index].first) / std::abs(inputpoints[candidates[i]].second - inputpoints[index].second);
        if (tempctan >= maxctan) {
            if (std::abs(tempctan - maxctan) < E) {
                if (inputpoints[candidates[i]].first > inputpoints[maxctanindex].first) maxctanindex = candidates[i];
            }
            else {
                maxctan = tempctan;
                maxctanindex = candidates[i];
            }
        }
    }
    convexhull.push_back(maxctanindex);
    index = maxctanindex;
    return true;
}

bool ConvexHullLowLeft(const std::vector<std::pair<double, double>>& inputpoints, size_t& index, std::vector<size_t>& convexhull) {
    double E = 1.e-5;
    std::vector<size_t> candidates;
    for (size_t i = 0; i < inputpoints.size(); i++) {
        if ((inputpoints[i].first <= inputpoints[index].first) && (inputpoints[i].second <= inputpoints[index].second) && (i != index)) candidates.push_back(i);
    }
    if (candidates.empty()) return false;
    double maxtan = 0;
    size_t maxtanindex = candidates[0];
    for (size_t i = 0; i < candidates.size(); i++) {
        double temptan = std::abs(inputpoints[candidates[i]].second - inputpoints[index].second) / std::abs(inputpoints[candidates[i]].first - inputpoints[index].first);
        if (temptan >= maxtan) {
            if (std::abs(temptan - maxtan) < E) {
                if (inputpoints[candidates[i]].first < inputpoints[maxtanindex].first) maxtanindex = candidates[i];
            }
            else {
                maxtan = temptan;
                maxtanindex = candidates[i];
            }
        }
    }
    convexhull.push_back(maxtanindex);
    index = maxtanindex;
    return true;
}

bool ConvexHullHighLeft(const std::vector<std::pair<double, double>>& inputpoints, size_t& index, std::vector<size_t>& convexhull) {
    double E = 1.e-5;
    std::vector<size_t> candidates;
    for (size_t i = 0; i < inputpoints.size(); i++) {
        if ((inputpoints[i].first <= inputpoints[index].first) && (inputpoints[i].second >= inputpoints[index].second) && (i != index)) candidates.push_back(i);
    }
    if (candidates.empty()) return false;
    double maxctan = 0;
    size_t maxctanindex = candidates[0];
    for (size_t i = 0; i < candidates.size(); i++) {
        double tempctan = std::abs(inputpoints[candidates[i]].first - inputpoints[index].first) / std::abs(inputpoints[candidates[i]].second - inputpoints[index].second);
        if (tempctan >= maxctan) {
            if (std::abs(tempctan - maxctan) < E) {
                if (inputpoints[candidates[i]].first < inputpoints[maxctanindex].first) maxctanindex = candidates[i];
            }
            else {
                maxctan = tempctan;
                maxctanindex = candidates[i];
            }
        }
    }
    convexhull.push_back(maxctanindex);
    index = maxctanindex;
    return true;
}

std::vector<size_t> FindConvexHull(const std::vector<std::pair<double, double>>& inputpoints) {
    double E = 1.e-5;
    std::vector<size_t> convexhull;
    double xmin = inputpoints[0].first;
    size_t index = 0;
    for (size_t i = 0; i < inputpoints.size(); i++) {
        if (inputpoints[i].first <= xmin) {
            if (std::abs(inputpoints[i].first - xmin) < E) {
                if (inputpoints[i].second > inputpoints[index].second) index = i;
            }
            else {
                xmin = inputpoints[i].first;
                index = i;
            }
        }

    }
    convexhull.push_back(index);
    while (ConvexHullHighRight(inputpoints, index, convexhull));
    while (ConvexHullLowRight(inputpoints, index, convexhull));
    while (ConvexHullLowLeft(inputpoints, index, convexhull));
    while (ConvexHullHighLeft(inputpoints, index, convexhull));
    convexhull.pop_back();
    return convexhull;
}

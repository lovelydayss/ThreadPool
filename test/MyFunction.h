
#ifndef MYFUNCTION_H
#define MYFUNCTION_H


#include <string>

int add(int i, int j) {
    return i + j;
}

static float minusBy5(float i) {
    return i - 5.0f;
}


class MyFunction {
public:
    std::string concat(std::string& str) const {
        return info_ + str;
    }

    static int multiply(int i, int j) {
        return i * j;
    }

private:
    std::string info_ = "MyFunction : ";
};

#endif //CGRAPH_MYFUNCTION_H
int foo() {
    return 3;
}

int main() {
    auto lambda = [] { return 3; };
    double direct_init = 3;
    double expression_init = 12 / 4;
    double function_init = foo();
    double lambda_init = lambda();
    double direct_lambda_init = [] { return 3; }();
    auto auto_init = 3;
}

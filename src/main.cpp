
#include <Eigen/Dense>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>

using namespace std;
using namespace Eigen;

int main() {
    const int imageSize = 1024; 

    VectorXd space = VectorXd::LinSpaced(imageSize, -1, 1);

    MatrixXd x(imageSize, imageSize), y(imageSize, imageSize);
    for (int i = 0; i < imageSize; ++i) {
        for (int j = 0; j < imageSize; ++j) {
            x(i, j) = space(j);
            y(i, j) = -space(i);
        }
    }

    ifstream file("prospero.vm");
    if (!file)
        throw runtime_error("Failed to open prospero.vm");

    stringstream buffer;
    buffer << file.rdbuf();
    string text = buffer.str();

    unordered_map<string, MatrixXd> v;
    string lastOut; 

    istringstream textStream(text);
    string line;
    while (getline(textStream, line)) {
        if (line.empty() || line[0] == '#')
            continue;
        istringstream lineStream(line);
        string out, op;
        lineStream >> out >> op;
        lastOut = out;
        vector<string> args;
        string token;
        while (lineStream >> token)
            args.push_back(token);

        if (op == "var-x") {
            v[out] = x;
        } else if (op == "var-y") {
            v[out] = y;
        } else if (op == "const") {
            if (args.empty())
                throw runtime_error("const expects an argument");
            double val = stod(args[0]);
            v[out] = MatrixXd::Constant(imageSize, imageSize, val);
        } else if (op == "add") {
            v[out] = v[args[0]] + v[args[1]];
        } else if (op == "sub") {
            v[out] = v[args[0]] - v[args[1]];
        } else if (op == "mul") {
            v[out] = v[args[0]].cwiseProduct(v[args[1]]);
        } else if (op == "max") {
            v[out] = v[args[0]].cwiseMax(v[args[1]]);
        } else if (op == "min") {
            v[out] = v[args[0]].cwiseMin(v[args[1]]);
        } else if (op == "neg") {
            v[out] = -v[args[0]];
        } else if (op == "square") {
            v[out] = v[args[0]].cwiseProduct(v[args[0]]);
        } else if (op == "sqrt") {
            v[out] = v[args[0]].cwiseSqrt();
        } else {
            throw runtime_error("unknown opcode '" + op + "'");
        }
    }

    MatrixXd result = v[lastOut];

    ofstream ofs("out.ppm", ios::binary);
    if (!ofs)
        throw runtime_error("Failed to open out.ppm for writing");

    ofs << "P5\n" << imageSize << " " << imageSize << "\n255\n";

    for (int i = 0; i < imageSize; ++i) {
        for (int j = 0; j < imageSize; ++j) {
            unsigned char pixel = (result(i, j) < 0) ? 255 : 0;
            ofs.write(reinterpret_cast<char*>(&pixel), 1);
        }
    }

    ofs.close();
    cout << "Image rendered to out.ppm\n";
    return 0;
}

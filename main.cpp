#include <bitset>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;

long long int maskInRange(long long int hex, int start, int end);
string convertToAsm(long long int hex);

int num = 1;
map<int, string> label;

class Format_R
{
public:
    long long int func7, rs2, rs1, func3, rd, opcode;

    Format_R(long long int hex)
    {
        this->opcode = maskInRange(hex, 0, 7);
        this->rd = maskInRange(hex, 7, 12);
        this->func3 = maskInRange(hex, 12, 15);
        this->rs1 = maskInRange(hex, 15, 20);
        this->rs2 = maskInRange(hex, 20, 25);
        this->func7 = maskInRange(hex, 25, 32);
    }

    string convert()
    {
        string eqAsm = "";
        if (func3 == 0x0)
        {
            if (func7 == 0x00)
            {
                eqAsm = "add x";
            }
            else if (func7 == 0x20)
            {
                eqAsm = "sub x";
            }
        }
        else if (func3 == 0x4 && func7 == 0x00)
        {
            eqAsm = "xor x";
        }
        else if (func3 == 0x6 && func7 == 0x00)
        {
            eqAsm = "or x";
        }
        else if (func3 == 0x7 && func7 == 0x00)
        {
            eqAsm = "and x";
        }
        else if (func3 == 0x1 && func7 == 0x00)
        {
            eqAsm = "sll x";
        }
        else if (func3 == 0x5 && func7 == 0x00)
        {
            eqAsm = "srl x";
        }
        else if (func3 == 0x5 && func7 == 0x20)
        {
            eqAsm = "sra x";
        }
        else if (func3 == 0x2 && func7 == 0x00)
        {
            eqAsm = "slt x";
        }
        else if (func3 == 0x3 && func7 == 0x00)
        {
            eqAsm = "sltu x";
        }
        if (eqAsm == "")
        {
            cout << "Not Valid R-Format instruction\nExiting\n";
            exit(0);
        }
        eqAsm += to_string(this->rd) + ", x" + to_string(this->rs1) + ", x" + to_string(this->rs2);
        // cout << eqAsm << '\n';
        return eqAsm;
    }
};

class Format_I
{
public:
    long long int imm, rs1, func3, rd, opcode;

    Format_I(long long int hex)
    {
        this->opcode = maskInRange(hex, 0, 7);
        this->rd = maskInRange(hex, 7, 12);
        this->func3 = maskInRange(hex, 12, 15);
        this->rs1 = maskInRange(hex, 15, 20);
        this->imm = maskInRange(hex, 20, 32);
        if (imm > 2047)
        {
            imm -= 4096;
        }
    }

    string convert()
    {
        string eqAsm = "";
        if (opcode == 0b0010011)
        {
            if (func3 == 0x0)
            {
                eqAsm = "addi x";
            }
            else if (func3 == 0x4)
            {
                eqAsm = "xori x";
            }
            else if (func3 == 0x6)
            {
                eqAsm = "ori x";
            }
            else if (func3 == 0x7)
            {
                eqAsm = "andi x";
            }
            else if (func3 == 0x1)
            {
                eqAsm = "slli x";
            }
            else if (func3 == 0x5 && imm > 1024)
            {
                imm -= 1024;
                eqAsm = "srai x";
            }
            else if (func3 == 0x5)
            {
                eqAsm = "srli x";
            }

            else if (func3 == 0x2)
            {
                eqAsm = "slti x";
            }
            else if (func3 == 0x3)
            {
                eqAsm = "sltiu x";
            }
            if (eqAsm == "")
            {
                cout << "Not Valid I-Format instruction\nExiting\n";
                exit(0);
            }
            eqAsm += to_string(this->rd) + ", x" + to_string(this->rs1) + ", " + to_string(this->imm);
        }
        else if (opcode == 0b0000011)
        {
            if (func3 == 0x0)
            {
                eqAsm = "lb x";
            }
            else if (func3 == 0x1)
            {
                eqAsm = "lh x";
            }
            else if (func3 == 0x2)
            {
                eqAsm = "lw x";
            }
            else if (func3 == 0x3)
            {
                eqAsm = "ld x";
            }
            else if (func3 == 0x4)
            {
                eqAsm = "lbu x";
            }
            else if (func3 == 0x5)
            {
                eqAsm = "lhu x";
            }
            else if (func3 == 0x6)
            {
                eqAsm = "lwu x";
            }
            if (eqAsm == "")
            {
                cout << "Not Valid I-Format instruction\nExiting\n";
                exit(0);
            }
            eqAsm += to_string(rd) + ", " + to_string(imm) + "(x" + to_string(rs1) + ")";
        }
        else if (opcode == 0b1100111)
        {
            eqAsm = "jalr x" + to_string(rd) + ", " + to_string(imm) + "(x" + to_string(rs1) + ")";
        }
        if (eqAsm == "")
        {
            cout << "Not Valid I-Format instruction\nExiting\n";
            exit(0);
        }
        // cout << eqAsm << '\n';
        return eqAsm;
    }
};

class Format_S
{
public:
    long long int imm, rs2, rs1, func3, opcode;
    Format_S(long long int hex)
    {
        this->opcode = maskInRange(hex, 0, 7);
        this->imm = maskInRange(hex, 7, 12);
        this->func3 = maskInRange(hex, 12, 15);
        this->rs1 = maskInRange(hex, 15, 20);
        this->rs2 = maskInRange(hex, 20, 25);
        this->imm += maskInRange(hex, 25, 32) << 5;
        if (imm > 2047)
        {
            imm -= 4096;
        }
        // cout << imm << '\n';
    }

    string convert()
    {
        string eqAsm = "";
        if (func3 == 0x0)
        {
            eqAsm = "sb x";
        }
        else if (func3 == 0x1)
        {
            eqAsm = "sh x";
        }
        else if (func3 == 0x2)
        {
            eqAsm = "sw x";
        }
        else if (func3 == 0x3)
        {
            eqAsm = "sd x";
        }
        else
        {
            cout << "Not Valid S-Format instruction\nExiting\n";
            exit(0);
        }
        eqAsm += to_string(rs2) + ", " + to_string(imm) + "(x" + to_string(rs1) + ")";
        return eqAsm;
    }
};

class Format_B
{
public:
    long long int opcode, imm, func3, rs1, rs2;
    Format_B(long long int hex)
    {
        opcode = maskInRange(hex, 0, 7);
        imm = maskInRange(hex, 7, 8) << 11;
        imm += maskInRange(hex, 8, 12) << 1;
        func3 = maskInRange(hex, 12, 15);
        rs1 = maskInRange(hex, 15, 20);
        rs2 = maskInRange(hex, 20, 25);
        imm += maskInRange(hex, 25, 31) << 5;
        imm += maskInRange(hex, 31, 32) << 12;
        if (imm > 4095)
        {
            imm -= 8192;
        }
        // cout << imm << '\n';
    }

    string convert()
    {
        string eqAsm = "";
        if (func3 == 0x0)
        {
            eqAsm = "beq x";
        }
        else if (func3 == 0x1)
        {
            eqAsm = "bne x";
        }
        else if (func3 == 0x4)
        {
            eqAsm = "blt x";
        }
        else if (func3 == 0x5)
        {
            eqAsm = "bge x";
        }
        else if (func3 == 0x6)
        {
            eqAsm = "bltu x";
        }
        else if (func3 == 0x7)
        {
            eqAsm = "bgeu x";
        }
        else
        {
            cout << "Not Valid B-Format instruction\nExiting\n";
            exit(0);
        }
        if (label.find(num + imm / 4) == label.end())
        {
            label[num + imm / 4] = "L" + to_string(label.size() + 1);
        }
        eqAsm += to_string(rs1) + ", x" + to_string(rs2) + ", " + label[num + imm / 4];

        return eqAsm;
    }
};

class Format_U
{
public:
    long long int opcode, rd, imm;

    Format_U(long long int hex)
    {
        opcode = maskInRange(hex, 0, 7);
        rd = maskInRange(hex, 7, 12);
        imm = maskInRange(hex, 12, 32);
        if (imm > 1048575)
        {
            imm -= 2097152;
        }
    }

    string convert()
    {
        stringstream stream;
        stream << hex << imm;
        string result(stream.str());
        return "lui x" + to_string(rd) + ", " + "0x" + result;
    }
};

class Format_J
{
public:
    long long int opcode, rd, imm;
    Format_J(long long int hex)
    {
        opcode = maskInRange(hex, 0, 7);
        rd = maskInRange(hex, 7, 12);
        imm = maskInRange(hex, 12, 20) << 12;
        imm += maskInRange(hex, 20, 21) << 11;
        imm += maskInRange(hex, 21, 31) << 1;
        imm += maskInRange(hex, 31, 32) << 20;
        if (imm > 1048575)
        {
            imm -= 2097152;
            // cout << bitset<32>(hex) << '\n';
        }
    }

    string convert()
    {
        string eqAsm = "";
        if (label.find(num + imm / 4) == label.end())
        {
            label[num + imm / 4] = "L" + to_string(label.size() + 1);
        }
        eqAsm = "jal x" + to_string(rd) + ", " + label[num + imm / 4];
        cout << num << " " << imm << " " << '\n';
        return eqAsm;
    }
};

// Extract out bits from start to end
long long int maskInRange(long long int hex, int start, int end)
{
    // hex >> (start) -> erase the bits upto start
    // ((1 << (end-start)) - 1) -> we get end-start set bits(2^(end-start)-1)
    // Taking bitwise and of the two numbers we get the required bits[start, end).
    return ((hex >> (start))) & ((1 << (end - start)) - 1);
}

string convertToAsm(long long int hex)
{
    long long int opcode = maskInRange(hex, 0, 7); // last 7 bits
    // cout << bitset<8>(opcode) << '\n';
    string eqAsm = "";
    if (opcode == 0b0110011)
    {
        Format_R current(hex);
        eqAsm = current.convert();
    }
    else if (opcode == 0b0010011)
    {
        Format_I current(hex);
        eqAsm = current.convert();
    }
    else if (opcode == 0b0000011)
    {
        Format_I current(hex);
        eqAsm = current.convert();
    }
    else if (opcode == 0b0100011)
    {
        Format_S current(hex);
        eqAsm = current.convert();
    }
    else if (opcode == 0b1100011)
    {
        Format_B current(hex);
        eqAsm = current.convert();
    }
    else if (opcode == 0b1101111)
    {
        Format_J current(hex);
        eqAsm = current.convert();
    }
    else if (opcode == 0b1100111)
    {
        Format_I current(hex);
        eqAsm = current.convert();
    }
    else if (opcode == 0b0110111)
    {
        Format_U current(hex);
        eqAsm = current.convert();
    }
    else
    {
        cout << "Found Unsupported Opcode\nExiting\n";
        exit(0);
    }

    return eqAsm;
}

int main()
{
    ifstream infile("Input.txt");
    ofstream outfile("Output.txt");
    vector<string> output;
    if (infile.is_open())
    {
        long long int line;
        while (infile >> hex >> line)
        {
            string eqAsm;
            eqAsm = convertToAsm(line);
            output.push_back(eqAsm);
            num++;
        }
    }
    for (int i = 0; i < output.size(); i++)
    {
        if (label.find(i + 1) != label.end())
        {
            output[i] = label[i + 1] + ": " + output[i];
        }
    }
    for (auto str : output)
        outfile << str << '\n';
    // cout << num << '\n';
}

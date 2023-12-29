#include <iostream>
#include <string.h>
#include <math.h>
#include <vector>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <limits>

class Step;
class ProcessBuilder;

class Step
{
public:
    virtual void execute() = 0;
    virtual std::string getType() const = 0;
    virtual bool userInteraction()
    {
        return true;
    }
    virtual ~Step() = default;
};

class TitleStep : public Step
{
private:
    std::string title;
    std::string subtitle;

public:
    TitleStep(const std::string &title, const std::string &subtitle) : title(title), subtitle(subtitle) {}

    void execute() override
    {
        std::cout << "Title: " << title << "\nSubtitle: " << subtitle << "\n";
    }

    std::string getType() const override
    {
        return "TITLE";
    }
};

class TextStep : public Step
{
private:
    std::string title;
    std::string copy;

public:
    TextStep(const std::string &title, const std::string &copy) : title(title), copy(copy) {}

    void execute() override
    {
        std::cout << "Title: " << title << "\nCopy: " << copy << "\n";
    }

    std::string getType() const override
    {
        return "TEXT";
    }

    bool userInteraction() override
    {
        // Add logic for user interaction specific to TextStep
        std::cout << "Press 'N' to skip to the next step or any other key to continue: ";
        char choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
        return (choice != 'N' && choice != 'n');
    }
};

class TextInputStep : public Step
{
private:
    std::string textInput;
    std::string description;

public:
    TextInputStep(const std::string &description) : description(description) {}

    void execute() override
    {
        std::cout << "Description: " << description << "\nEnter text: ";
        std::cin >> textInput;
    }

    std::string getType() const override
    {
        return "TEXT INPUT";
    }
};

class NumberInputStep : public Step
{
private:
    std::string description;
    float numberInput;

public:
    NumberInputStep(const std::string &description) : description(description) {}

    void execute() override
    {
        std::cout << "Description: " << description << "\nEnter number: ";
        std::cin >> numberInput;

        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            throw std::invalid_argument("Invalid input. Please enter a valid number.");
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::string getType() const override
    {
        return "NUMBER INPUT";
    }
};

class CalculusStep : public Step
{
private:
    int steps;
    std::string operation;
    std::vector<float> operands;

public:
    CalculusStep(int steps, const std::string &operation) : steps(steps), operation(operation) {}

    void execute() override
    {
        std::cout << "Performing calculus operations:\n ";

        if (operands.size() < 2)
        {
            std::cerr << "Error!!!\n";
            return;
        }

        float result;
        if (operation == "+")
        {
            result = operands[0] + operands[1];
        }
        else if (operation == "-")
        {
            result = operands[0] - operands[1];
        }
        else if (operation == "*")
        {
            result = operands[0] * operands[1];
        }
        else if (operation == "/")
        {
            if (operands[1] != 0)
            {
                result = operands[0] / operands[1];
            }
            else
            {
                std::cerr << "Error!\n";
                return;
            }
        }
        else if (operation == "min")
        {
            result = std::min(operands[0], operands[1]);
        }
        else if (operation == "max")
        {
            result = std::max(operands[0], operands[1]);
        }
        else
        {
            std::cerr << "Erorr!! Unsupported operation\n";
            return;
        }
        std::cout << "Result: " << result << "\n";
    }

    void addOperand(float operand)
    {
        operands.push_back(operand);
    }

    std::string getType() const override
    {
        return "CALCULUS";
    }
};

class DisplayStep : public Step
{
private:
    int step;

public:
    DisplayStep(int step) : step(step) {}

    void execute() override
    {
        std::cout << "Displaying content for step " << step << " \n";

        std::vector<std::string> stepResults;

        if (step < stepResults.size())
        {
            std::cout << "Content: " << stepResults[step - 1] << "\n";
        }
        else
        {
            std::cerr << "Error! Content not available for step " << step << "!\n";
        }
    }

    std::string getType() const override
    {
        return "DISPLAY";
    }
};

class TextFileInputStep : public Step
{
private:
    std::string description;
    std::string file_name;

public:
    TextFileInputStep(const std::string &description, const std::string &file_name) : description(description), file_name(file_name) {}

    void execute() override
    {
        std::cout << "Executing TextFileInputText: " << description << "\n";

        // read the content from the file we will create
    }

    std::string getType() const override
    {
        return "TEXT FILE INPUT";
    }
};

class CSVFileInputStep : public Step
{
private:
    std::string description;
    std::string file_name;

public:
    CSVFileInputStep(const std::string &description, const std::string &file_name) : description(description), file_name(file_name) {}

    void execute() override
    {
        // read the content from the CSV file
    }
    std::string getType() const override
    {
        return "CSV FILE INPUT";
    }
};

class OutputStep : public Step
{
private:
    int step;
    std::string fileName;
    std::string title;
    std::string description;

public:
    OutputStep(int step, const std::string &fileName, const std::string &title, const std::string &description) : fileName(fileName), title(title), description(description) {}

    void execute() override
    {
        std::cout << "Executing OutputStep for step " << step << "\n";
        std::vector<std::string> stepResults;

        if (step <= stepResults.size())
        {
            // open the file for writing (then we execute the other operations on the file)
        }
    }

    std::string getType() const override
    {
        return "OUTPUT";
    }
};

class EndStep : public Step
{
public:
    void execute() override
    {
        std::cout << "End of the flow\n";
    }

    std::string getType() const override
    {
        return "END";
    }
};

class ProcessBuilder
{
private:
    std::vector<Step *> steps;
    std::string flowName;
    time_t creationTimestamp;

public:
    ProcessBuilder()
    {
        creationTimestamp = time(nullptr);
    }

    ~ProcessBuilder()
    {
        for (Step *step : steps)
        {
            delete step;
        }
    }

    void setFlowName(const std::string &name)
    {
        flowName = name;
    }

    void addStep(Step *step)
    {
        steps.push_back(step);
    }

    void runFlow()
    {
        for (Step *step : steps)
        {
            step->execute();

            // Check if user wants to skip the step
            if (!step->userInteraction())
            {
                std::cout << "Skipping to the next step..." << std::endl;
                continue;
            }

            // Continue to the next step
            std::cout << "Press enter to proceed to the next step...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        std::cout << "Flow completed." << std::endl;
    }

    std::string getCreationTimestamp() const
    {
        return std::asctime(std::localtime(&creationTimestamp)); // Convert timestamp to a readable string
    }
};

int main()
{
    ProcessBuilder process;

    std::string flowName;
    std::cout << "Enter the name for your flow: ";
    std::getline(std::cin, flowName);
    process.setFlowName(flowName);

    process.addStep(new TextStep("Welcome", "This is a sample flow!"));
    process.addStep(new TextInputStep("Enter your name: "));
    process.addStep(new NumberInputStep("Enter a number: "));
    process.addStep(new EndStep());

    process.runFlow();

    // Get and display the creation timestamp
    std::cout << "Flow '" << flowName << "' created at: " << process.getCreationTimestamp() << std::endl;
    return 0;
}
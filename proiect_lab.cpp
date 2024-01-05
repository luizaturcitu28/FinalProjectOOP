#include <iostream>
#include <string.h>
#include <math.h>
#include <vector>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <sstream>
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
    virtual void displayDescription() const
    {
        std::cout << "No description available for this step" << std::endl;
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
    void displayDescription() const override
    {
        std::cout << "This step displays a title and a subtitle" << std::endl;
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

    void displayDescription() const override
    {
        std::cout << "This step displays a title and a copy" << std::endl;
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

    void displayDescription() const override
    {
        std::cout << "This step displays a text and a description" << std::endl;
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

    void displayDescription() const override
    {
        std::cout << "This step displays a description and a number" << std::endl;
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

    void displayDescription() const override
    {
        std::cout << "This step displays the number of steps, the operation and the operands" << std::endl;
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

    void displayDescription() const override
    {
        std::cout << "This step displays the number of the step" << std::endl;
    }
};

class TextFileInputStep : public Step
{
private:
    std::string description;
    std::string fileName;
    std::string fileContent;

public:
    TextFileInputStep(const std::string &description, const std::string &file_name) : description(description), fileName(fileName) {}

    void execute() override
    {
        std::cout << "Description: " << description << "\nFile name: " << fileName << std::endl;
        std::ifstream inputFile(fileName);

        if (inputFile.is_open())
        {
            // read the content from the file
            std::string line;
            while (std::getline(inputFile, line))
            {
                fileContent += line + '\n';
            }
            std::cout << "File content: \n"
                      << fileContent << std::endl;
            // close the file
            inputFile.close();
        }
        else
        {
            std::cerr << "Unable to open the file: " << fileName << std::endl;
        }
    }

    std::string getType() const override
    {
        return "TEXT FILE INPUT";
    }

    void displayDescription() const override
    {
        std::cout << "This step displays a description and the name of the file" << std::endl;
    }
};

class CSVFileInputStep : public Step
{
private:
    std::string description;
    std::string file_name;
    std::vector<std::vector<std::string>> csvData;

public:
    CSVFileInputStep(const std::string &description, const std::string &file_name) : description(description), file_name(file_name) {}

    void execute() override
    {
        std::cout << "Description: " << description << "\nFile name: " << file_name << std::endl;
        std::ifstream inputFile(file_name);

        if (inputFile.is_open())
        {
            std::string line;
            while (std::getline(inputFile, line))
            {
                std::vector<std::string> row;
                std::stringstream ss(line);
                std::string cell;

                while (std::getline(ss, cell, ','))
                {
                    row.push_back(cell);
                }
                csvData.push_back(row);
            }

            // display the CSV data
            std::cout << "CSV content: " << std::endl;
            for (const auto &row : csvData)
            {
                for (const auto &cell : row)
                {
                    std::cout << cell << " | ";
                }
                std::cout << std::endl;
            }

            // close file
            inputFile.close();
        }
        else
        {
            std::cerr << "Unable to open file: " << file_name << std::endl;
        }
    }
    std::string getType() const override
    {
        return "CSV FILE INPUT";
    }

    void displayDescription() const override
    {
        std::cout << "This step displays a description and the name of the file" << std::endl;
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

    void displayDescription() const override
    {
        std::cout << "This step displays the step number, the name of the file, the title and the description" << std::endl;
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

    void displayDescription() const override
    {
        std::cout << "This step is the end of the flow" << std::endl;
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

    template <typename T, typename... Args>
    void addStep(Args &&...args)
    {
        steps.push_back(new T(std::forward<Args>(args)...));
    }

    void runFlow()
    {
        std::cout << "Running flow '" << flowName << "' created at: " << std::asctime(std::localtime(&creationTimestamp));

        for (Step *step : steps)
        {
            std::cout << "Executing step: " << step->getType() << std::endl;

            // Execute the step
            step->execute();

            // Check if the user wants to skip the step
            if (!step->userInteraction())
            {
                std::cout << "Skipping to the next step..." << std::endl;
                continue;
            }

            // Wait for user confirmation to proceed to the next step
            std::cout << "Press enter to proceed to the next step...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        std::cout << "Flow completed." << std::endl;
    }

    void displayAvailableSteps() const
    {
        std::cout << "Available steps: " << std::endl;
        for (const Step *step : steps)
        {
            std::cout << "- Type: " << step->getType() << std::endl;
            step->displayDescription();
            std::cout << std::endl;
        }
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

    process.displayAvailableSteps();

    // dynamically add steps to the flow
    char addMore;
    do
    {
        std::cout << "Enter the type of step to add (TITLE, TEXT, NUMBER, etc.): ";
        std::string stepType;
        std::cin >> stepType;

        if (stepType == "TITLE")
        {
            std::string title, subtitle;
            std::cout << "Enter title: ";
            std::cin >> title;
            std::cout << "Enter subtitle: ";
            std::cin >> subtitle;
            process.addStep<TitleStep>(title, subtitle);
        }

        else if (stepType == "TEXT")
        {
            std::string title;
            std::string copy;
            std::cout << "Enter title: ";
            std::cin >> title;
            std::cout << "Enter copy: ";
            std::cin >> copy;
            process.addStep<TextStep>(title, copy);
        }

        else if (stepType == "TEXT INPUT")
        {
            std::string textInput;
            std::string description;
            std::cout << "Enter a text input: ";
            std::cin >> textInput;
            std::cout << "Enter a description: ";
            std::cin >> description;
            process.addStep<TextInputStep>(textInput, description);
        }

        else if (stepType == "NUMBER INPUT")
        {
            std::string description;
            float numberInput;
            std::cout << "Enter a description: ";
            std::cin >> description;
            std::cout << "Enter a number input: ";
            std::cin >> numberInput;
            process.addStep<NumberInputStep>(description, numberInput);
        }

        else if (stepType == "CALCULUS")
        {
            int steps;
            std::string operation;
            std::cout << "Enter the number of the steps: ";
            std::cin >> steps;
            std::cout << "Enter the operation: ";
            std::cin >> operation;
            process.addStep<NumberInputStep>(steps, operation);
        }

        else if (stepType == "DISPLAY")
        {
            int step;
            std::cout << "Enter a step to display: (1, 2, ..., 10)";
            std::cin >> step;
            process.addStep<DisplayStep>(step);
        }

        else if (stepType == "TEXT FILE INPUT")
        {
            std::string description;
            std::string fileName;
            std::string fileContent;
            std::cout << "Enter a description for this step: ";
            std::cin >> description;
            std::cout << "Enter the name of this file: ";
            std::cout << fileName;
            process.addStep<TextFileInputStep>(description, fileName);
        }

        else if (stepType == "CSV FILE INPUT")
        {
            std::string description;
            std::string file_name;
            std::cout << "Enter a description for this step: ";
            std::cin >> description;
            std::cout << "Enter the name of this file: ";
            std::cin >> file_name;
            process.addStep<CSVFileInputStep>(description, file_name);
        }

        else if (stepType == "OUTPUT")
        {
            int step;
            std::string fileName;
            std::string title;
            std::string description;
            std::cout << "Enter the number of the step (1, 2, ..., 10): ";
            std::cin >> step;
            std::cout << "Enter the name of the file from where you want to extract the informations: ";
            std::cin >> fileName;
            std::cout << "Enter a title for this step: ";
            std::cin >> title;
            std::cout << "Enter a description for this step: ";
            std::cin >> description;
            process.addStep<OutputStep>(step, fileName, title, description);
        }

        else
        {
            process.addStep<EndStep>();
        }

        std::cout << "Do you want to add more steps? (y/n): ";
        std::cin >> addMore;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    } while (addMore == 'y' || addMore == 'Y');

    std::string fileName = "flow.txt";
    std::string file_name = "flow.csv";

    process.runFlow();

    // Get and display the creation timestamp
    std::cout << "Flow '" << flowName << "' created at: " << process.getCreationTimestamp() << std::endl;
    return 0;
}
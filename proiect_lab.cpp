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
#include <iomanip>
#include <unordered_map>
using namespace std;

class Step;
class ProcessBuilder;

enum class OperationType
{
    Addition,
    Substraction,
    Multiplication,
    Division,
    Minimum,
    Maximum
};

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

    bool userInteraction() override
    {
        // Add logic for user interaction specific to TextStep
        std::cout << "Press 'N' to skip to the next step or any other key to continue: ";
        char choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
        return (choice != 'N' && choice != 'n');
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

    bool userInteraction() override
    {
        // Add logic for user interaction specific to TextStep
        std::cout << "Press 'N' to skip to the next step or any other key to continue: ";
        char choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
        return (choice != 'N' && choice != 'n');
    }

    void displayDescription() const override
    {
        std::cout << "This step displays a title and a copy" << std::endl;
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
        std::cout << "Description: " << description << std::endl;
        std::cout << "Enter text: ";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
        std::getline(std::cin, textInput);
    }

    std::string getType() const override
    {
        return "TEXT INPUT";
    }

    bool userInteraction() override
    {
        std::cout << "Press 'N' to skip to the next step or any other key to continue: ";
        char choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
        return (choice != 'N' && choice != 'n');
    }

    void displayDescription() const override
    {
        std::cout << "This step displays a text and a description" << std::endl;
    }

    std::string getTextInput() const
    {
        return textInput;
    }
};

class CSVInputStep : public Step
{
private:
    std::string description;
    std::string CSVInput;
    std::string fileName;

public:
    CSVInputStep(const std::string &description) : description(description) {}

    void execute() override
    {
        std::cout << "Description: " << description << std::endl;
        std::cout << "Enter CSV data: ";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
        std::getline(std::cin, CSVInput);

        std::cout << "Enter the filename to save the CSV data: ";
        std::getline(std::cin, fileName);

        // save CSV data to the file
        saveCsvToFile();
    }

    std::string getType() const override
    {
        return "CSV INPUT";
    }

    bool userInteraction() override
    {
        std::cout << "Press 'N' to skip to the next step or any other key to continue: ";
        char choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
        return (choice != 'N' && choice != 'n');
    }

    void displayDescription() const override
    {
        std::cout << "Expected input: " << description << std::endl;
    }

    std::string getCSVInput() const
    {
        return CSVInput;
    }

    std::string getFileName() const
    {
        return fileName;
    }

    void saveCsvToFile() const
    {
        // Optional: Save CSV data to the specified file
        std::ofstream outputFile(fileName);
        if (outputFile.is_open())
        {
            outputFile << CSVInput;
            outputFile.close();
            std::cout << "CSV data saved to file: " << fileName << std::endl;
        }
        else
        {
            std::cerr << "Error: Unable to save CSV data to file '" << fileName << "'." << std::endl;
        }
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
        std::cout << "Description: " << description << std::endl;
        std::cout << "Enter a number: ";
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

    bool userInteraction() override
    {
        std::cout << "Press 'N' to skip to the next step or any other key to continue: ";
        char choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
        return (choice != 'N' && choice != 'n');
    }

    void displayDescription() const override
    {
        std::cout << "This step displays a description and a number" << std::endl;
    }

    float getNumberInput() const
    {
        return numberInput;
    }
};

class CalculusStep : public Step
{
private:
    std::vector<NumberInputStep *> previousSteps;
    std::vector<char> operations;
    float result;
    OperationType operationType;

public:
    CalculusStep(const std::vector<NumberInputStep *> &steps, const std::vector<char> &ops, OperationType opType) : previousSteps(steps), operations(ops), result(0.0f), operationType(opType) {}

    void execute() override
    {
        std::cout << "Expression: ";
        for (size_t i = 0; i < previousSteps.size(); ++i)
        {
            std::cout << "Step: " << i + 1;
            if (i < operations.size())
            {
                std::cout << " " << operations[i] << " ";
            }
        }
        std::cout << std::endl;

        // perform the operation
        result = previousSteps[0]->getNumberInput();

        for (size_t i = 0; i < operations.size(); ++i)
        {
            float operand = previousSteps[i + 1]->getNumberInput();

            switch (operationType)
            {
            case OperationType::Addition:
                result += operand;
                break;
            case OperationType::Substraction:
                result -= operand;
                break;
            case OperationType::Multiplication:
                result *= operand;
                break;
            case OperationType::Division:
                result /= operand;
                break;
            case OperationType::Minimum:
                result = std::min(result, operand);
                break;
            case OperationType::Maximum:
                result = std::max(result, operand);
                break;
            default:
                std::cerr << "Invalid operation: " << operations[i] << std::endl;
                break;
            }
        }
        std::cout << "Result: " << result << std::endl;
    }

    std::string getType() const override
    {
        return "CALCULUS";
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

    void displayDescription() const override
    {
        std::cout << "This step displays the number of steps, the operation and the operands" << std::endl;
    }

    float getResult() const
    {
        return result;
    }
};

class DisplayStep : public Step
{
private:
    Step *previousStep;

public:
    DisplayStep(Step *prevStep) : previousStep(prevStep) {}

    void execute() override
    {
        std::cout << "Displaying information from the previous step:... " << std::endl;
        if (previousStep->getType() == "TEXT INPUT")
        {
            TextInputStep *textInputStep = dynamic_cast<TextInputStep *>(previousStep);
            if (textInputStep)
            {
                std::cout << "Text Input Content: " << textInputStep->getTextInput() << std::endl;
            }
        }
        else if (previousStep->getType() == "CSV INPUT")
        {
            CSVInputStep *csvInputStep = dynamic_cast<CSVInputStep *>(previousStep);
            if (csvInputStep)
            {
                std::string fileName = csvInputStep->getFileName();
                displayFileContent(fileName);
            }
        }
        else
        {
            std::cout << "Cannot display information. Previous step type not supported" << std::endl;
        }
    }

    std::string getType() const override
    {
        return "DISPLAY";
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

    void displayFileContent(const std::string &fileName) const
    {
        std::ifstream file(fileName);
        if (file.is_open())
        {
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::cout << "File Content:" << std::endl
                      << buffer.str() << std::endl;
            file.close();
        }
        else
        {
            std::cout << "Error: Unable to open file '" << fileName << "'." << std::endl;
        }
    }

    void displayDescription() const override
    {
        std::cout << "This step displays informations about the other steps" << std::endl;
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

    bool userInteraction() override
    {
        // Add logic for user interaction specific to TextStep
        std::cout << "Press 'N' to skip to the next step or any other key to continue: ";
        char choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
        return (choice != 'N' && choice != 'n');
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

    bool userInteraction() override
    {
        // Add logic for user interaction specific to TextStep
        std::cout << "Press 'N' to skip to the next step or any other key to continue: ";
        char choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
        return (choice != 'N' && choice != 'n');
    }

    void displayDescription() const override
    {
        std::cout << "This step displays a description and the name of the file" << std::endl;
    }
};

class OutputStep : public Step
{
private:
    int stepNumber;
    std::string fileName;
    std::string title;
    std::string description;
    std::vector<std::string> contentFromPreviousSteps;

public:
    OutputStep(int stepNumber, const std::string &fileName, const std::string &title, const std::string &description, const std::vector<std::string> &contentFromPreviousStepss) : stepNumber(stepNumber), fileName(fileName), title(title), description(description), contentFromPreviousSteps(contentFromPreviousSteps) {}

    void execute() override
    {
        std::cout << "Executing OutputStep: " << std::endl;
        std::ofstream outputFile(fileName);
        if (outputFile.is_open())
        {
            outputFile << "Step Number: " << stepNumber << std::endl;
            outputFile << "Title: " << title << std::endl;
            outputFile << "Description: " << description << std::endl;

            // Add content from previous steps
            for (const std::string &content : contentFromPreviousSteps)
            {
                outputFile << content << std::endl;
            }

            outputFile.close();
            std::cout << "Output file '" << fileName << "' generated successfully" << std::endl;
        }
        else
        {
            std::cerr << "Erorr!! Unable to create output file '" << fileName << "'!" << std::endl;
        }
    }

    std::string getType() const override
    {
        return "OUTPUT";
    }

    void displayDescription() const override
    {
        std::cout << "Step number: " << stepNumber << std::endl;
        std::cout << "File Name: " << fileName << std::endl;
        std::cout << "Title: " << title << std::endl;
        std::cout << "Description: " << description << std::endl;

        // display content from previous steps
        for (const std::string &content : contentFromPreviousSteps)
        {
            std::cout << content << std::endl;
        }
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

    // Analytics
    int startCount;
    int completionCount;
    std::unordered_map<std::string, int> screenSkipCount;
    std::unordered_map<std::string, int> errorScreenCount;
    int totalErrorCount;

public:
    ProcessBuilder()
    {
        creationTimestamp = time(nullptr); // set the creation time stamp to the current time
        startCount = 0;
        completionCount = 0;
        totalErrorCount = 0;
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

    std::string getFlowName() const
    {
        return flowName;
    }

    template <typename T, typename... Args>
    void addStep(Args &&...args)
    {
        steps.push_back(new T(std::forward<Args>(args)...));
    }

    void runFlow()
    {
        startCount++;
        std::cout << "Running flow '" << flowName << "' created at: " << std::asctime(std::localtime(&creationTimestamp));

        std::vector<std::string> contentFromPreviousSteps;

        for (Step *step : steps)
        {
            std::cout << "Executing step: " << step->getType() << std::endl;

            // check if the user wants to skip the step
            if (step->userInteraction())
            {
                // execute the step
                step->execute();
                // If the step is an OUTPUT step, store its content for future steps
                if (step->getType() == "OUTPUT")
                {
                    const OutputStep *outputStep = dynamic_cast<const OutputStep *>(step);
                    if (outputStep)
                    {
                        // Extract content from the OUTPUT step and store it
                        outputStep->displayDescription(); // Display OUTPUT step details
                        contentFromPreviousSteps.push_back("Content from output step");
                    }
                }
            }
            else
            {
                std::cout << "Skipping to the next step..." << std::endl;
                screenSkipCount[step->getType()]++;
            }

            // Wait for user confirmation to proceed to the next step
            std::cout << "Press enter to proceed to the next step...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        const CalculusStep *lastCalculusStep = dynamic_cast<const CalculusStep *>(steps.back());
        if (lastCalculusStep)
        {
            std::cout << "Final Result: " << lastCalculusStep->getResult() << std::endl;
        }

        completionCount++;
        std::cout << "Flow completed." << std::endl;
    }

    // Error handling
    void reportError(const std::string &stepType)
    {
        errorScreenCount[stepType]++;
        totalErrorCount++;
    }

    // Analytics
    void displayAnalytics() const
    {
        std::cout << "Analytics for flow '" << flowName << "':" << std::endl;
        std::cout << "Flow started: " << startCount << " times" << std::endl;
        std::cout << "Flow completed: " << completionCount << " times" << std::endl;

        std::cout << "Screen skip counts:" << std::endl;
        for (const auto &entry : screenSkipCount)
        {
            std::cout << entry.first << ": " << entry.second << " times" << std::endl;
        }

        std::cout << "Error screen counts:" << std::endl;
        for (const auto &entry : errorScreenCount)
        {
            std::cout << entry.first << ": " << entry.second << " times" << std::endl;
        }

        if (completionCount > 0)
        {
            double averageErrors = static_cast<double>(totalErrorCount) / completionCount;
            std::cout << "Average number of errors per flow completed: " << averageErrors << std::endl;
        }
        else
        {
            std::cout << "Average number of errors per flow completed: N/A (no completions)" << std::endl;
        }
    }

    // function to delete a flow
    void deleteFlow(const std::string &flowToDelete)
    {
        // check if the flow we eant to delete matches the current flow
        if (flowToDelete == flowName)
        {
            std::cout << "Deleting flow '" << flowToDelete << "'..." << std::endl;
            // clean up allocated steps
            for (Step *step : steps)
            {
                delete step;
            }
            // clear the step vector
            steps.clear();
            // reset analytics
            startCount = 0;
            completionCount = 0;
            screenSkipCount.clear();
            errorScreenCount.clear();
            totalErrorCount = 0;
            // reset flow name
            flowName.clear();
        }
        else
        {
            std::cerr << "Error!!! Flow '" << flowToDelete << "' not found" << std::endl;
        }
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
            std::string description;
            std::cout << "Enter description for text input step: ";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
            std::getline(std::cin, description);
            process.addStep<TextInputStep>(description);
        }

        else if (stepType == "CSV INPUT")
        {
            std::string description;
            std::cout << "Enter description for CSV INPUT step: ";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
            std::getline(std::cin, description);
            process.addStep<CSVInputStep>(description);
        }

        else if (stepType == "NUMBER INPUT")
        {
            std::string description;
            std::cout << "Enter a description for number input step: ";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
            std::getline(std::cin, description);
            process.addStep<NumberInputStep>(description);
        }

        else if (stepType == "CALCULUS")
        {
            std::vector<NumberInputStep *> previousSteps;
            std::vector<char> operations;
            std::cout << "Enter the expression ('3 + 4 * 2'): ";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::string expression;
            std::getline(std::cin, expression);
            std::istringstream iss(expression);
            std::string token;
            while (iss >> token)
            {
                if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1])))
                {
                    std::string description = "Operand" + token;
                    NumberInputStep *numStep = new NumberInputStep(description);
                    previousSteps.push_back(numStep);
                }
                else
                {
                    operations.push_back(token[0]);
                }
            }

            std::cout << "Choose the operation type: " << std::endl;
            std::cout << "1. Addition" << std::endl;
            std::cout << "2. Substraction" << std::endl;
            std::cout << "3. Multiplication" << std::endl;
            std::cout << "4. Division" << std::endl;
            std::cout << "5. Minimum" << std::endl;
            std::cout << "6. Maximum" << std::endl;
            int choice;
            std::cout << "Enter your choice (1-6): ";
            std::cin >> choice;
            OperationType opType;
            switch (choice)
            {
            case 1:
                opType = OperationType::Addition;
                break;
            case 2:
                opType = OperationType::Substraction;
                break;
            case 3:
                opType = OperationType::Multiplication;
                break;
            case 4:
                opType = OperationType::Division;
                break;
            case 5:
                opType = OperationType::Minimum;
                break;
            case 6:
                opType = OperationType::Maximum;
                break;
            default:
                std::cerr << "Invalid choice. Defaulting to addition." << std::endl;
                opType = OperationType::Addition;
                break;
            }
            process.addStep<CalculusStep>(previousSteps, operations, opType);

            for (NumberInputStep *numStep : previousSteps)
            {
                delete numStep;
            }
        }

        else if (stepType == "DISPLAY")
        {
            std::cout << "Enter the type of of the previous step(TEXT INPUT, CVS INPUT etc.)";
            std::string prevStepType;
            std::cin >> prevStepType;
            if (prevStepType == "TEXT INPUT" || prevStepType == "CSV INPUT")
            {
                // Create a dummy step of the previous type for demonstration purposes
                Step *dummyPrevStep = (prevStepType == "TEXT INPUT") ? static_cast<Step *>(new TextInputStep("")) : static_cast<Step *>(new CSVInputStep(""));
                process.addStep<DisplayStep>(dummyPrevStep);
            }
            else
            {
                std::cout << "Invalid previous step type. Display step requires TEXT INPUT or CSV INPUT." << std::endl;
            }
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
            int stepNumber;
            std::string fileName;
            std::string title;
            std::string description;
            std::vector<std::string> contentFromPreviousSteps;

            std::cout << "Enter step number for OUTPUT step: ";
            std::cin >> stepNumber;

            std::cout << "Enter file name for OUTPUT step: ";
            std::cin >> fileName;

            std::cout << "Enter title for OUTPUT step: ";
            std::cin >> title;

            std::cout << "Enter description for OUTPUT step: ";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::getline(std::cin, description);

            contentFromPreviousSteps.push_back("Content from previous step 1");
            contentFromPreviousSteps.push_back("Content from previous step 2");

            process.addStep<OutputStep>(stepNumber, fileName, title, description, contentFromPreviousSteps);
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
    process.displayAnalytics();

    process.deleteFlow(flowName);
    return 0;
}
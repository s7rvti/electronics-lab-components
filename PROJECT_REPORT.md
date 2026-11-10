

# 1. Project Information

## 1.1 Project Title

Project title: Electronics Lab Components Inventory Tracker

## 1.2 Engineering Domain

Engineering domain:

Electronics

## 1.3 Team Details

| Field | Details |
|---|---|
| Student name | Shruti Lonkar |
| Roll number |612507185|
| Division / batch |EnTc Div 2|
| Repository name |electronics-lab-components|
| GitHub repository link | |

## 1.4 Individual Project Declaration

I confirm that this project is my individual work. I understand the design, source code, unit tests, Makefile and documentation included in this repository, and I am prepared to explain or modify any part during evaluation.

Student signature or acknowledgement: 

[Enter acknowledgement]

---

# 2. Engineering Scenario

## 2.1 Scenario Description

Describe the selected engineering scenario.

Include:

- Where the scenario occurs
- Who is involved
- Current working method
- Information that must be managed
- Problems in the current method

Project-specific description:

[Write the scenario description here]

---

## 2.2 Problem Statement

Use the following structure:

In [domain or environment], [target users] currently face [specific problem]. The current method results in [errors, delays, limitations or difficulties]. The proposed software will provide [major capabilities] so that users can [expected result or benefit].

Final problem statement:

[Write the final problem statement here]

---

## 2.3 Proposed Solution

Describe the proposed application.

Explain:

- What the application will do
- How it will address the problem
- What information it will manage
- What important calculations or decisions it will perform
- What reports or outputs it will generate

Proposed solution:

[Write the proposed solution here]

---

## 2.4 Target Users

| User Type | Responsibility | How the User Will Use the Application |
|---|---|---|
| | | |
| | | |
| | | |

---

# 3. Project Scope

## 3.1 In-Scope Features

List the features that will be implemented.

1. [Feature]
2. [Feature]
3. [Feature]
4. [Feature]
5. [Feature]

## 3.2 Out-of-Scope Features

List related features that will not be implemented.

1. [Feature not included]
2. [Feature not included]
3. [Feature not included]

## 3.3 Assumptions

List the assumptions made during project design.

1. [Assumption]
2. [Assumption]
3. [Assumption]

## 3.4 Constraints

List project constraints.

Examples include limited time, file-based storage, command-line interface or fixed maximum records.

1. [Constraint]
2. [Constraint]
3. [Constraint]

---

# 4. Requirements

## 4.1 Functional Requirements

| ID | Functional Requirement | Priority | Verification Method |
|---|---|---|---|
| FR-01 | The system shall... | High | Demonstration / Test |
| FR-02 | The system shall... | High | Demonstration / Test |
| FR-03 | The system shall... | Medium | Demonstration / Test |
| FR-04 | The system shall... | Medium | Demonstration / Test |
| FR-05 | The system shall... | Low | Demonstration / Test |

Add additional rows as required.

## 4.2 Non-Functional Requirements

| ID | Non-Functional Requirement | Verification Method |
|---|---|---|
| NFR-01 | The source code shall compile without warnings. | Build output |
| NFR-02 | The project shall use a modular multi-file structure. | Code review |
| NFR-03 | The application shall handle invalid input without crashing. | Negative testing |
| NFR-04 | Important functions shall be tested using gTest. | Unit-test output |
| NFR-05 | The project shall build using a Makefile. | Build demonstration |
| NFR-06 | The project shall be maintained in a GitHub repository. | Repository review |

Add project-specific non-functional requirements if required.

## 4.3 Requirement Traceability

| Requirement ID | Module | Function or Feature | Test ID |
|---|---|---|---|
| FR-01 | | | |
| FR-02 | | | |
| FR-03 | | | |
| FR-04 | | | |

Complete this table after the detailed design and test plan are prepared.

---

# 5. High-Level Architecture

## 5.1 Architecture Description

Explain the overall organisation of the software.

[Write the architecture description here]

## 5.2 Architecture Diagram

Insert the project architecture diagram below.

[Insert diagram or image reference here]

Example image reference:

    ![Software Architecture](architecture.png)

## 5.3 High-Level Workflow

Describe or draw the workflow from application start to exit.

[Insert the high-level workflow here]

---

# 6. Module Design

## 6.1 Module Summary

| Module Name | Source File | Header File | Responsibility |
|---|---|---|---|
| Main application | main.c | Not applicable | Controls the high-level application flow |
| | | | |
| | | | |
| | | | |
| | | | |

## 6.2 Detailed Module Description

Complete the following subsection for every module.

### Module: [Module Name]

Source file:

[filename.c]

Header file:

[filename.h]

Purpose:

[Describe the purpose of the module]

Responsibilities:

- [Responsibility]
- [Responsibility]
- [Responsibility]

Public functions:

- [Function]
- [Function]

Data used or owned:

[Describe data used by the module]

Dependencies:

[List dependent modules]

Possible errors:

[List possible errors or status codes]

Repeat this subsection for all modules.

---

# 7. Data Design

## 7.1 Structure Definitions

### Structure: [Structure Name]

Purpose:

[Explain what the structure represents]

Proposed definition:

    typedef struct
    {
        /* Add members */
    } StructureName_t;

Member details:

| Member | C Type | Valid Range or Size | Description |
|---|---|---|---|
| | | | |
| | | | |
| | | | |

Repeat for all structures.

## 7.2 Enumeration Definitions

### Enumeration: [Enumeration Name]

Purpose:

[Explain the fixed states or categories]

Proposed definition:

    typedef enum
    {
        /* Add enumeration constants */
    } EnumerationName_t;

| Enumeration Constant | Meaning |
|---|---|
| | |
| | |
| | |

Repeat for all enumerations.

## 7.3 Constants

| Constant | Value | Purpose |
|---|---:|---|
| | | |
| | | |
| | | |

## 7.4 Data Dictionary

| Data Item | C Type | Valid Range or Size | Validation Rule | Description |
|---|---|---|---|---|
| | | | | |
| | | | | |
| | | | | |
| | | | | |

---

# 8. Function Design

Complete one subsection for every important function.

## 8.1 Function: [Function Name]

Module:

[Module name]

Prototype:

    [return_type] functionName([parameters]);

Purpose:

[Describe the single responsibility of the function]

Parameters:

| Parameter | C Type | Direction | Description | Valid Values |
|---|---|---|---|---|
| | | Input / Output / Input-Output | | |
| | | | | |

Return type:

[Return type]

Return values:

| Return Value | Meaning |
|---|---|
| | |
| | |
| | |

Preconditions:

- [Condition]
- [Condition]

Postconditions:

- [Condition]
- [Condition]

Errors handled:

- [Error]
- [Error]

Related requirements:

- [FR-XX]

Planned unit tests:

- [UT-XX]
- [UT-XX]

Repeat this section for every important function.

---

# 9. Input-Validation Plan

| Input | Source | Validation Rule | Invalid Example | Expected Error Behaviour |
|---|---|---|---|---|
| | User / File | | | |
| | | | | |
| | | | | |
| | | | | |

Explain the common validation strategy:

[Write the validation strategy here]

---

# 10. File-Handling Design

## 10.1 File Summary

| File | Purpose | Format | Created By | Read By | Updated By |
|---|---|---|---|---|---|
| | | Text / Binary | | | |
| | | | | | |
| | | | | | |

## 10.2 Record Format

Describe the data format used in each file.

[Write the record format here]

## 10.3 File Operations

| Operation | Function or Module | C Library Function | Failure Handling |
|---|---|---|---|
| Create or open | | | |
| Read | | | |
| Write | | | |
| Update | | | |
| Close | | | |

## 10.4 File Error Conditions

| Error Condition | Detection Method | Expected Behaviour |
|---|---|---|
| File does not exist | | |
| File cannot be opened | | |
| File is empty | | |
| File contains invalid data | | |
| Read or write fails | | |

---

# 11. Error-Handling Strategy

## 11.1 Status Representation

Proposed status type:

    typedef enum
    {
        STATUS_SUCCESS = 0,
        /* Add project-specific status values */
    } Status_t;

## 11.2 Error Catalogue

| Error ID | Error Condition | Detection Location | Returned Status | User-Facing Behaviour |
|---|---|---|---|---|
| ERR-01 | | | | |
| ERR-02 | | | | |
| ERR-03 | | | | |
| ERR-04 | | | | |

## 11.3 Recovery Strategy

Explain how the application continues or exits safely after errors.

[Write the recovery strategy here]

---

# 12. Coding Standards Plan

## 12.1 Naming Conventions

| Item | Convention | Example |
|---|---|---|
| Source files | | |
| Header files | | |
| Functions | | |
| Local variables | | |
| Structures | | |
| Enumerations | | |
| Constants | | |

## 12.2 Formatting Rules

- Indentation: [Specify spaces]
- Brace style: [Specify style]
- Maximum line length: [Specify if applicable]
- Compiler warning flags: [Specify flags]
- Comment style: [Specify style]

## 12.3 Function Rules

- [Rule]
- [Rule]
- [Rule]

## 12.4 Prohibited Practices

- [Practice to avoid]
- [Practice to avoid]
- [Practice to avoid]

---

# 13. Application Flow

## 13.1 Main Application Flowchart

[Insert the main application flowchart here]

## 13.2 Operation Flow: [Operation Name]

[Insert the flowchart or step sequence]

Repeat for complex operations.

---

# 14. Unit-Test Plan

## 14.1 Functions Selected for Testing

| Function | Module | Reason for Testing |
|---|---|---|
| | | |
| | | |
| | | |

## 14.2 Test Cases

| Test ID | Function | Category | Input or Condition | Expected Result | Requirement ID |
|---|---|---|---|---|---|
| UT-01 | | Positive | | | |
| UT-02 | | Negative | | | |
| UT-03 | | Boundary | | | |
| UT-04 | | Positive | | | |
| UT-05 | | Negative | | | |
| UT-06 | | Boundary | | | |
| UT-07 | | Positive | | | |
| UT-08 | | Negative | | | |
| UT-09 | | Boundary | | | |
| UT-10 | | | | | |

Add additional rows as required.

## 14.3 Test Data and Setup

Describe:

- Test input data
- Temporary files
- Initial conditions
- Cleanup required after each test

[Test setup description]

## 14.4 Expected Test Command

    make test

---

# 15. Makefile Design

## 15.1 Required Targets

| Target | Purpose | Expected Output |
|---|---|---|
| all | Build the application | |
| run | Run the application | |
| test | Build and execute unit tests | |
| clean | Remove generated files | |

## 15.2 Build Configuration

| Item | Planned Value |
|---|---|
| C compiler | |
| C++ compiler | |
| C compiler flags | |
| C++ compiler flags | |
| Include paths | |
| gTest libraries | |
| Application executable | |
| Test executable | |

---

# 16. Repository Structure

Replace the example names with project-specific names.

    project-name/
    |
    |-- src/
    |   |-- main.c
    |   `-- module.c
    |
    |-- include/
    |   `-- module.h
    |
    |-- tests/
    |   `-- test_module.cpp
    |
    |-- data/
    |   `-- sample_data.txt
    |
    |-- docs/
    |   |-- project_proposal.md
    |   |-- detailed_design.md
    |   `-- architecture.png
    |
    |-- Makefile
    |-- README.md
    |-- .gitignore
    `-- LICENSE

Final project structure:

[Insert the final repository structure here]

---

# 17. Git and GitHub Plan

## 17.1 Repository Information

| Field | Value |
|---|---|
| Repository name | |
| Repository visibility | Public / As instructed |
| Main branch | |
| GitHub link | |

## 17.2 Planned Commit Milestones

| Milestone | Planned Commit Message | Status |
|---|---|---|
| Repository creation | Create initial project structure | Planned / Complete |
| Data design | | |
| Core module | | |
| File handling | | |
| Unit testing | | |
| Makefile | | |
| Documentation | | |

## 17.3 Files to Ignore

Planned .gitignore entries:

    build/
    *.o
    *.out
    *.exe
    *.log
    core

Add or remove entries according to the project.

---

# 18. Implementation Status

| Work Item | Status | Evidence or Commit |
|---|---|---|
| Proposal | Not Started / In Progress / Complete | |
| Detailed design | | |
| Module 1 | | |
| Module 2 | | |
| Module 3 | | |
| File handling | | |
| Unit tests | | |
| Makefile | | |
| README | | |
| Repository verification | | |

---

# 19. Test Execution Results

## 19.1 Build Result

Command:

    make all

Result:

[Pass / Fail]

Warnings or errors:

[Record output or state None]

## 19.2 Unit-Test Result

Command:

    make test

Total tests:

[Number]

Passed:

[Number]

Failed:

[Number]

Test-output evidence:

[Insert concise output or screenshot reference]

## 19.3 Application Test Results

| Scenario | Input or Condition | Expected Result | Actual Result | Status |
|---|---|---|---|---|
| Normal use | | | | Pass / Fail |
| Invalid input | | | | |
| Boundary value | | | | |
| Missing record | | | | |
| File failure | | | | |

---

# 20. README Content Checklist

Confirm that README.md contains:

- [ ] Project title
- [ ] Engineering domain
- [ ] Problem statement
- [ ] Proposed solution
- [ ] Features
- [ ] Architecture diagram
- [ ] Module descriptions
- [ ] Folder structure
- [ ] C concepts used
- [ ] Coding standards
- [ ] Prerequisites
- [ ] Build instructions
- [ ] Run instructions
- [ ] Test instructions
- [ ] Sample input
- [ ] Sample output
- [ ] Unit-testing summary
- [ ] Error-handling summary
- [ ] Team members
- [ ] Known limitations
- [ ] Future enhancements
- [ ] GitHub repository link

---

# 21. Demonstration Plan

| Sequence | Demonstration Item | Approximate Time |
|---:|---|---|
| 1 | Introduce engineering scenario | |
| 2 | Explain problem statement | |
| 3 | Present requirements | |
| 4 | Explain architecture | |
| 5 | Show repository structure | |
| 6 | Build using make all | |
| 7 | Run using make run | |
| 8 | Demonstrate major features | |
| 9 | Demonstrate invalid-input handling | |
| 10 | Execute make test | |
| 11 | Show commit history | |
| 12 | Explain limitations and improvements | |

---

# 22. Reflection

## 22.1 What Worked Well?

[Write the response here]

## 22.2 Challenges Faced

[Write the response here]

## 22.3 How Were the Challenges Resolved?

[Write the response here]

## 22.4 What Did You Learn?

Discuss learning related to:

- C programming
- Modular design
- Coding standards
- Unit testing
- Makefiles
- Git
- GitHub

[Write the response here]

## 22.5 Known Limitations

1. [Limitation]
2. [Limitation]
3. [Limitation]

## 22.6 Future Enhancements

1. [Enhancement]
2. [Enhancement]
3. [Enhancement]

---

# 23. Final Submission Checklist

## Proposal and Design

- [ ] Project title is clear.
- [ ] Engineering scenario is realistic.
- [ ] Problem statement is specific.
- [ ] Target users are identified.
- [ ] Scope is defined.
- [ ] Functional requirements are documented.
- [ ] Non-functional requirements are documented.
- [ ] Architecture diagram is included.
- [ ] Modules are identified.
- [ ] Data structures are designed.
- [ ] Major functions are specified.
- [ ] Error-handling strategy is defined.
- [ ] Unit-test plan is complete.

## Implementation

- [ ] Multiple C source files are used.
- [ ] Header files are provided.
- [ ] main.c contains high-level control only.
- [ ] Structures are used meaningfully.
- [ ] Functions have clear parameters and return values.
- [ ] Input validation is implemented.
- [ ] File operations are checked.
- [ ] Errors are handled.
- [ ] Coding standards are followed.
- [ ] Code compiles without warnings.
- [ ] No unnecessary or unused code remains.

## Unit Testing

- [ ] gTest is used.
- [ ] At least three important functions are tested.
- [ ] At least ten meaningful tests are included.
- [ ] Positive tests are included.
- [ ] Negative tests are included.
- [ ] Boundary tests are included.
- [ ] Tests are independent and repeatable.
- [ ] make test works.
- [ ] All tests pass.

## Makefile

- [ ] make all works.
- [ ] make run works.
- [ ] make test works.
- [ ] make clean works.

## Git and GitHub

- [ ] Git was used throughout development.
- [ ] Multiple meaningful commits are present.
- [ ] .gitignore is configured.
- [ ] Generated files are not committed.
- [ ] GitHub contains the complete project.
- [ ] The repository was cloned and verified independently.
- [ ] The correct repository link is submitted.

## Documentation

- [ ] README is complete.
- [ ] Architecture is documented.
- [ ] Repository structure is documented.
- [ ] Build instructions are correct.
- [ ] Run instructions are correct.
- [ ] Test instructions are correct.
- [ ] Sample output is included.
- [ ] Limitations are documented.
- [ ] Future enhancements are documented.

---

# 24. Final Submission Information

Project title:

[Enter title]

Engineering domain:

[Enter domain]

Student name:

[Enter student name]

Roll number:

[Enter roll number]

Division / batch:

[Enter division or batch]

GitHub repository link:

[Enter accessible GitHub repository link]

Project proposal location:

    docs/project_proposal.md

Detailed design location:

    docs/detailed_design.md

README location:

    README.md

Build command:

    make all

Run command:

    make run

Unit-test command:

    make test

Clean command:

    make clean

Submission date:

[Enter date]

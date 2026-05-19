# AI Usage Report – Phase 1 & Phase 2

## Tools Used
During the development of this project, I used an AI assistant (ChatGPT) as a support tool to help understand concepts, debug code, and improve implementation quality. All final decisions and integration into the project were done manually after reviewing and testing the generated suggestions.

---

## 2. Phase 1 – AI Usage

In Phase 1, I used AI mainly for the following tasks:

### 2.1 Parsing and filtering logic
- I requested help to design and implement:
  - `parse_condition()` to split filter strings of the form `field:operator:value`
  - `match_condition()` to evaluate conditions on structured report data

The AI helped generate a base implementation using `strtok()` and string comparison functions. I reviewed and tested the functions, and adjusted them to ensure:
- Correct type conversion (atoi / atol)
- Proper handling of all operators (==, !=, <, >, <=, >=)
- Correct comparison for both numeric and string fields

### 2.2 File permission handling
I used AI assistance to better understand:
- How to interpret `st_mode` permission bits
- How to convert permission bits into symbolic format.


---

### 2.3 Debugging file operations
AI was used to help debug issues related to:
- binary file reading/writing using `read()` and `write()`
- shifting records when removing entries using `lseek()` and `ftruncate()`

I verified all fixes using test cases.

---

## 3. Phase 2 – AI Usage

In Phase 2, AI was used to help design process and signal-based communication.

---

### 3.1 Monitor process design
AI helped design the structure of:
- `monitor_reports` program
- creation of `.monitor_pid` file
- signal handling using `sigaction()`

I implemented and adjusted:
- SIGUSR1 handler (report notification)
- SIGINT handler (clean shutdown and file removal)

---

### 3.2 Signal communication (SIGUSR1)
AI helped clarify:
- how to use `kill()` to send signals between processes
- how to safely read PID from a file
- how to handle errors when PID file is missing or invalid

I implemented full error handling:
- missing `.monitor_pid`
- invalid PID values
- failed `kill()` calls

Each case logs an appropriate message in `logged_district`.

---

### 3.3 Process creation for remove_district
AI assisted in implementing:
- `fork()` to create a child process
- `execlp()` to run `rm -rf <directory>`
- `waitpid()` to ensure proper synchronization

I ensured safety by:
- validating district ID input
- preventing path traversal
- deleting symlinks only after successful removal

---

## 4. Issues Found and Fixes

During development, I identified and fixed several issues in AI-generated suggestions:

- Missing error handling in file operations
- Incomplete validation for PID reading
- Unsafe assumptions about file existence
- Need for explicit logging on failure cases
- Required separation of monitor process and city_manager execution

## 5. What I Learned

Through this project, I learned:

- How UNIX file permissions work in practice
- How to use low-level system calls (`open`, `read`, `write`, `lseek`)
- How binary file manipulation works with structured data
- How processes communicate using signals (`SIGUSR1`, `SIGINT`)
- How `fork()` and `exec()` are used to create new processes
- How to safely integrate AI-generated code into a real system after validation


## 6. Conclusion
AI was used as a support tool for learning and guidance. All final implementations were reviewed, corrected, and integrated manually. The project was fully tested in a UNIX environment to ensure correctness and compliance with requirements.


## 3. Phase 3 – AI Usage

In Phase 3, AI was used to design and debug the extended multi-process system introducing pipes, process coordination, and external executables.

3.1 city_hub architecture design

AI assistance was used to clarify the overall structure of the city_hub program, particularly:

* Designing the interactive command-line interface
* Understanding the use of fork() for creating background processes
* Structuring the execution of an external monitor program using execl()
* Coordinating parent and child process responsibilities

This helped in organizing the system so that process creation and execution flow were correctly separated between the hub and monitor components.

⸻

3.2 Inter-process communication using pipes

AI was used to better understand and implement pipe-based communication between processes.

Support was helpful in:

* Setting up pipes using pipe()
* Redirecting output streams using dup2()
* Ensuring correct closure of unused file descriptors in parent and child processes
* Handling continuous reading of variable-length output using read()

This improved the reliability of communication between the monitor process and the hub interface.

⸻

3.3 calculate_scores implementation

AI assistance was used during the development of the calculate_scores feature to support:

* Parsing multiple command-line arguments using strtok()
* Spawning multiple scorer processes using fork()
* Executing external programs using execl()
* Collecting and aggregating output from multiple processes using pipes

It was particularly useful in resolving issues related to:

* Argument parsing for multiple districts
* Correct sequencing of process creation and output collection
* Ensuring proper synchronization between child processes and the hub

⸻

3.4 Debugging and validation

AI was used during debugging to help identify issues such as:

* Incorrect pipe handling causing missing output
* Improper ordering of fork() and exec() calls
* Output buffering affecting real-time display
* Process synchronization issues when handling multiple child processes

All corrections were validated through manual testing in a UNIX environment.

⸻

3.5 System concepts reinforced

Through development of this phase, the following system-level concepts were reinforced:

* UNIX process model (fork, exec, wait)
* Inter-process communication using pipes
* Output redirection with dup2
* Multi-process coordination and execution flow

⸻

4. Conclusion

Across all phases, external tools were used as a support mechanism for understanding and debugging system-level concepts. Final implementations were developed, tested, and integrated manually to ensure correctness and compliance with assignment requirements.

⸻
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
# Partial-Wiper Malware PoC: A Study in Partial Destruction

## ⚠️ IMPORTANT: EDUCATIONAL PURPOSE ONLY ⚠️

**This project is a Proof-of-Concept (PoC) developed strictly for academic and research purposes. It is intended for use in controlled, ethical security research environments to study malware behavior and improve defensive solutions**

## Overview

This repository contains a Proof-of-Concept (PoC) for a wiper malware, written in C++. The primary goal of this project is to demonstrate the alarming effectiveness of a simple approach to data destruction that currently evades detection by the vast majority of commercial antivirus (AV) engines on VirusTotal.

Unlike sophisticated malware samples which employ anti-analysis techniques (e.g., packing, obfuscation, VM/Sandbox evasion), this PoC achieves a very low detection rate by relying on three simple key strategies:

1.  **Partial Overwrite:** Instead of fully overwriting files with null-bytes, which is a common signature for wipers, this malware overwrites only **3/8ths (37.5%)** of each target file's content. This is often enough to corrupt the file structure and make it irrecoverable), while being a less common pattern for AV signatures, both statically (using YARA with opcode patterns) or dynamically.
2.  **Memory Mapping:** Instead of using normal file-interaction APIs (like WriteFile), the malware uses memory-mapping to wipe the files. This technique is both faster for large files & more evasive. This approach is increasingly common in ransomware (used, for example, by Maze), so implementing it here seemed to be an interesting idea.
3.  **Targeted File Selection:** The malware does not wipe the entire filesystem. Instead, it searches for files that are most likely to be valuable, thereby maximizing potential damage with minimal activity. This also helps avoid triggering detections based on mass file operations. The search is done by looking for the largest files that have been accessed in the last year.

## Key Findings & Security Implications

As of the time of its initial publication of this repo (September 26th 2025), this PoC was submitted to VirusTotal for analysis.

**The result: 2 detections out of 72 AV engines.**
*(Detections: MaxSecure - `Trojan.Malware.300983.susgen`, SecureAge - `Malicious`)*

<img width="1146" height="194" alt="VT" src="https://github.com/user-attachments/assets/6cf8bded-d751-44bf-8aee-89109d9770ba" />


Furthermore, VirusTotal's behavioral analysis did not flag the sample as malicious at all, indicating that its dynamic runtime behavior also evaded detection by automated sandbox solutions (Jujubox, Zenbox & CAPE Sandbox).
<img width="1147" height="510" alt="image" src="https://github.com/user-attachments/assets/6a727c8c-6468-47ef-9c3a-98cf3af8a5df" />

This extremely low detection rate is significant for the security community because:

*   **It highlights a critical gap in detection capabilities.** The industry's focus on complete file overwrites has potentially left a blind spot for partial data destruction techniques.
*   **It demonstrates that sophistication is not always necessary for evasion.** Even without any advanced anti-analysis features, a cleverly designed attack vector can remain largely undetected.
*   **The existing detections appear to be generic.** The signatures from MaxSecure and SecureAge are highly generic (`susgen` for "suspicious generic," and the broad classification of `Malicious`). This suggests that these detections may be based on heuristic luck or overly broad rules rather than a specific, robust signature for this type of partial overwrite behavior. It is most probably not a tailored identification of the threat.

This PoC serves as a stark reminder that both wiper and ransomware malware could adopt similar partial destruction methods to evade detection while still causing significant harm. Defensive solutions need to evolve to detect anomalous patterns of file modification and data destruction, not just the "noisy" traditional methods.

## Technical Info

### File Targeting Logic
The malware recursively iterates the `C:\` drive but avoids the root directory (`C:\`) itself to prevent immediate destruction of critical system files. It then:
1.  Gathers a list of files from subdirectories
2.  Filters this list to include only files accessed within the last year (a potential indicator of active, valuable files)
3.  Selects the **top 1000 largest files** from this filtered list, operating under the assumption that larger files often contain more critical data (databases, projects, archives, etc.)

### Data Destruction Method
For each of the 1000 targeted files, the malware:
1.  Opens the file with write permissions
2.  Calculates the amount of data to overwrite: `(3 * file_size) / 8`
3.  Overwrites that portion of the file from the 2nd eighth with null-bytes
4.  Closes the file, leaving the remaining portion intact but the file effectively corrupted

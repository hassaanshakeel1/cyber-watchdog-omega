/**
 * ======================================================================================
 * CYBER-WATCHDOG: OMEGA PROTOCOL (PLAGIARISM EDITION)
 * ======================================================================================
 * ARCHITECTS: HASSAAN SHAKEEL & AHMAD ALI
 * STATUS: DEPLOYMENT READY (OPTIMIZED MEMORY & SPEED)
 * NEW FEATURE: AUTOMATED PLAGIARISM DETECTION (JACCARD INDEX)
 * ======================================================================================
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cctype>
#include <iomanip>
#include <map> 
#include <cmath>
#include <algorithm>
#include <set>
#include <ctime>
#include <regex>
#include <numeric>

using namespace std;

// ==========================================
// 1. CONFIGURATION
// ==========================================
const string DEV_1 = "HASSAAN SHAKEEL";
const string DEV_2 = "AHMAD ALI";

// ==========================================
// 2. DATA STRUCTURES
// ==========================================
struct ThreatMatch {
    string name;
    string type;
    string matchedData;
    int riskScore;
};

struct IOC {
    string type; 
    string value;
};

struct BehavioralTag {
    string category; 
    string apiName;
    string mitrePhase; 
};

struct ZoneEntropy {
    double header;
    double code;
    double data;
    double overlay;
};

struct ScanResult {
    string filename;
    string fileType;
    double entropy;
    ZoneEntropy zoneEnt;
    double threatScore;
    unsigned long integrityHash;
    string fuzzySig;
    string capHash;
    vector<ThreatMatch> detections;
    vector<string> hiddenArtifacts;
    vector<IOC> iocs;
    vector<BehavioralTag> behavior;
    vector<string> antiEvasion;
    bool isSpoofed;
    bool isPacked;
    bool hasShellcode;
    bool isRansomware; 
    string ransomwareReason;
};

struct CompareData {
    double similarity;      // Byte-level similarity
    double plagiarismScore; // Word-level plagiarism score (New)
    bool integrityMatch;
    ScanResult suspect;
    ScanResult original;
    string hexDump;
};

// ==========================================
// 3. OMEGA ENGINE (Deep Scan Logic)
// ==========================================
class OmegaEngine {
public:
    static unsigned long djb2(const string& str) {
        unsigned long hash = 5381;
        for(char c : str) hash = ((hash << 5) + hash) + c;
        return hash;
    }

    static string generateFuzzyHash(const string& data) {
        if(data.empty()) return "00";
        static const string b64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        string sig;
        sig.reserve(data.length() / 64 + 1);
        
        int blockSize = (data.length() / 64) + 1;
        unsigned long currentVal = 0;
        for(size_t i=0; i<data.length(); i++) {
            currentVal = ((currentVal << 5) + currentVal) + data[i];
            if(i % blockSize == 0) {
                sig += b64[currentVal % 64];
                currentVal = 0;
            }
        }
        return sig;
    }

    // --- NEW PLAGIARISM LOGIC (JACCARD INDEX) ---
    static double calculatePlagiarism(const string& s1, const string& s2) {
        auto tokenize = [](const string& text) {
            set<string> tokens;
            string word;
            for (char c : text) {
                if (isalnum(c)) word += tolower(c);
                else if (!word.empty()) {
                    if (word.length() > 2) tokens.insert(word); // Ignore tiny words
                    word = "";
                }
            }
            if (!word.empty() && word.length() > 2) tokens.insert(word);
            return tokens;
        };

        set<string> set1 = tokenize(s1);
        set<string> set2 = tokenize(s2);

        if (set1.empty() || set2.empty()) return 0.0;

        vector<string> intersection;
        set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), back_inserter(intersection));

        vector<string> unionSet;
        set_union(set1.begin(), set1.end(), set2.begin(), set2.end(), back_inserter(unionSet));

        if (unionSet.empty()) return 0.0;
        return ((double)intersection.size() / (double)unionSet.size()) * 100.0;
    }

    static int levenshteinDist(const string& s1, const string& s2) {
        if (s1.size() > s2.size()) return levenshteinDist(s2, s1);
        const size_t m = s1.size();
        const size_t n = s2.size();
        vector<int> prev(m + 1);
        vector<int> curr(m + 1);
        iota(prev.begin(), prev.end(), 0);
        for (size_t j = 1; j <= n; ++j) {
            curr[0] = j;
            for (size_t i = 1; i <= m; ++i) {
                int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
                curr[i] = min(min(curr[i - 1] + 1, prev[i] + 1), prev[i - 1] + cost);
            }
            prev = curr;
        }
        return prev[m];
    }

    static double calcEntropy(const string& data) {
        if (data.empty()) return 0.0;
        vector<unsigned long> freq(256, 0); 
        for (unsigned char c : data) freq[c]++;
        
        double ent = 0.0, len = (double)data.length();
        double log2 = log(2.0);
        
        for (unsigned long count : freq) {
            if (count > 0) {
                double p = (double)count / len;
                ent -= p * (log(p) / log2);
            }
        }
        return ent;
    }

    static ZoneEntropy analyzeZones(const string& data) {
        ZoneEntropy z = {0,0,0,0};
        if(data.length() < 100) return z;
        size_t q = data.length() / 4;
        z.header = calcEntropy(data.substr(0, q));
        z.code = calcEntropy(data.substr(q, q));
        z.data = calcEntropy(data.substr(q*2, q));
        z.overlay = calcEntropy(data.substr(q*3));
        return z;
    }

    static string generateCapHash(const vector<BehavioralTag>& tags) {
        if(tags.empty()) return "0000-NONE";
        vector<string> sorted;
        sorted.reserve(tags.size());
        for(const auto& t : tags) sorted.push_back(t.apiName);
        sort(sorted.begin(), sorted.end());
        string raw = "";
        for(const auto& s : sorted) raw += s;
        unsigned long h = djb2(raw);
        stringstream ss; ss << hex << h;
        return ss.str();
    }

    static string toUpper(string s) {
        for(auto& c : s) c = toupper(c);
        return s;
    }

    static vector<BehavioralTag> analyzeImports(const string& data) {
        vector<BehavioralTag> tags;
        struct ApiDef { string name; string cat; string mitre; };
        static const vector<ApiDef> library = {
            {"VirtualAlloc", "Memory", "Defense Evasion"},
            {"WriteProcessMemory", "Injection", "Privilege Escalation"},
            {"CreateRemoteThread", "Injection", "Defense Evasion"},
            {"GetAsyncKeyState", "Keylogger", "Collection"},
            {"SetWindowsHookEx", "Keylogger", "Collection"},
            {"InternetOpen", "Network", "C2"},
            {"HttpSendRequest", "Network", "Exfiltration"},
            {"RegCreateKey", "Persistence", "Persistence"},
            {"ShellExecute", "Execution", "Execution"},
            {"IsDebuggerPresent", "Anti-Debug", "Defense Evasion"},
            {"CheckRemoteDebugger", "Anti-Debug", "Defense Evasion"},
            {"CryptEncrypt", "Crypto", "Impact"},
            {"URLDownloadToFile", "Downloader", "Resource Dev"}
        };
        for(const auto& api : library) {
            if(data.find(api.name) != string::npos) {
                tags.push_back(BehavioralTag{api.cat, api.name, api.mitre});
            }
        }
        return tags;
    }

    static vector<string> checkEvasion(const string& data) {
        vector<string> hits;
        vector<string> artefacts = {"VMware", "VBox", "QEMU", "Wireshark", "OllyDbg", "SandBox"};
        string up;
        up.reserve(data.length());
        for(char c : data) if(isprint(c)) up += (char)toupper(c);

        for(const string& a : artefacts) {
            string ua = toUpper(a);
            if(up.find(ua) != string::npos) hits.push_back(a);
        }
        return hits;
    }

    static pair<bool, string> checkRansomware(const string& data) {
        static const vector<string> keywords = {"ENCRYPTED", "BITCOIN", "RECOVERY", "DECRYPT", "ONION", "TOR BROWSER", "YOUR FILES"};
        string upData;
        upData.reserve(data.length()); 
        for(char c : data) if(isprint(c)) upData += (char)toupper(c);
        for(const string& kw : keywords) {
            if(upData.find(kw) != string::npos) return {true, "Keyword Found: " + kw};
        }
        return {false, ""};
    }

    static vector<IOC> extractIOCs(const string& data) {
        vector<IOC> found;
        try {
            static const regex ip_regex(R"((\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}))");
            static const regex url_regex(R"((http[s]?:\/\/[a-zA-Z0-9\.\-\/]+))");
            static const regex email_regex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");

            auto wb = sregex_iterator(data.begin(), data.end(), ip_regex);
            auto we = sregex_iterator();
            for (auto i = wb; i != we; ++i) if(found.size()<10) found.push_back(IOC{"IP", i->str()});

            wb = sregex_iterator(data.begin(), data.end(), url_regex);
            for (auto i = wb; i != we; ++i) if(found.size()<20) found.push_back(IOC{"URL", i->str()});

            wb = sregex_iterator(data.begin(), data.end(), email_regex);
            for (auto i = wb; i != we; ++i) if(found.size()<25) found.push_back(IOC{"Email", i->str()});
        } catch(...) {}
        return found;
    }

    static ScanResult analyze(const string& data, string fname) {
        ScanResult r;
        r.filename = fname;
        r.entropy = calcEntropy(data);
        r.zoneEnt = analyzeZones(data); 
        r.threatScore = 0;
        r.isSpoofed = false;
        r.isPacked = false;
        r.hasShellcode = false;
        r.isRansomware = false;
        r.integrityHash = djb2(data);
        r.fuzzySig = generateFuzzyHash(data);
        r.iocs = extractIOCs(data);
        r.behavior = analyzeImports(data);
        r.antiEvasion = checkEvasion(data); 
        r.capHash = generateCapHash(r.behavior); 

        pair<bool, string> kwCheck = checkRansomware(data);
        bool cryptoBehavior = false;
        for(const auto& b : r.behavior) {
            if(b.category == "Crypto" || b.category == "Injection") cryptoBehavior = true;
        }
        bool highEntropy = (r.entropy > 6.8);

        if (kwCheck.first) { r.isRansomware = true; r.ransomwareReason = kwCheck.second; r.threatScore += 50; }
        else if (highEntropy) { r.isRansomware = true; r.ransomwareReason = "High Entropy Detected (>6.8)"; r.threatScore += 30; }
        else if (cryptoBehavior) { r.isRansomware = true; r.ransomwareReason = "Suspicious Crypto/Injection API"; r.threatScore += 40; }
        else { r.ransomwareReason = "Clean"; }

        if(r.isRansomware) r.detections.push_back(ThreatMatch{"Ransomware Indicator", "Heuristic", r.ransomwareReason, 80});

        if(!r.antiEvasion.empty()) {
            r.threatScore += 25;
            for(const auto& s : r.antiEvasion) r.detections.push_back(ThreatMatch{"Anti-Analysis Artifact", "Evasion", s, 25});
        }

        if (data.size() > 4) {
            if (data.substr(0, 2) == "MZ") r.fileType = "Windows Executable";
            else if (data.substr(0, 2) == "PK") r.fileType = "Compressed Archive (ZIP)";
            else if (data.substr(0, 4) == "%PDF") r.fileType = "PDF Document";
            else if (data.substr(0, 3) == "ELF") r.fileType = "Linux Executable (ELF)";
            else r.fileType = "Raw Data Stream";
        }

        string upName = toUpper(fname);
        if (r.fileType == "Windows Executable") {
            if (upName.find(".TXT") != string::npos || upName.find(".JPG") != string::npos || upName.find(".PDF") != string::npos) {
                r.isSpoofed = true;
                r.threatScore = 100;
                r.detections.push_back(ThreatMatch{"Extension Spoofing", "Deception", "MZ Header vs Extension", 100});
            }
        }

        vector<string> artifacts;
        string buffer = "";
        buffer.reserve(100); 
        for(size_t i=0; i<data.length(); i++) {
            if(isprint(data[i])) buffer += data[i];
            else {
                if(buffer.length() > 3) artifacts.push_back(toUpper(buffer));
                buffer = "";
            }
        }

        const char* bad_exts[] = {".EXE", ".VBS", ".BAT", ".SCR", ".PS1", ".CMD", ".MSI", ".COM"};
        for(const string& art : artifacts) {
            for(int j=0; j<8; j++) {
                if(art.find(bad_exts[j]) != string::npos) {
                    r.threatScore = 100;
                    r.detections.push_back(ThreatMatch{"Hidden Executable", "Payload", art, 100});
                    r.hiddenArtifacts.push_back(art);
                }
            }
        }

        struct Sig { string pat; string name; string type; int risk; };
        static const vector<Sig> db = {
            {"WANNACRY", "WannaCry Ransomware", "Ransomware", 100},
            {"POWERSHELL", "PowerShell Script", "Loader", 50},
            {"CMD.EXE", "Command Shell", "System", 40},
            {"TVQQA", "Base64 EXE Header", "Obfuscation", 80},
            {"MIMIKATZ", "Mimikatz Credential Dump", "Stealer", 100},
            {"X5O!P%@AP[4", "EICAR Test String", "Test File", 100} 
        };

        for (const string& art : artifacts) {
            for (const auto& sig : db) {
                if (art.find(sig.pat) != string::npos) {
                    bool exists = false;
                    for(auto& d : r.detections) if(d.name == sig.name) exists = true;
                    if(!exists) {
                        r.threatScore += sig.risk;
                        r.detections.push_back(ThreatMatch{sig.name, sig.type, art, sig.risk});
                    }
                }
            }
        }

        if (r.zoneEnt.overlay > 7.5 && r.fileType.find("Archive") == string::npos) {
            r.isPacked = true;
            r.threatScore += 30;
            r.detections.push_back(ThreatMatch{"High Overlay Entropy", "Heuristic", "Packed/Encrypted Overlay", 30});
        }

        if(r.threatScore > 100) r.threatScore = 100;
        return r;
    }

    static CompareData compare(const string& cA, string nA, const string& cB, string nB) {
        CompareData res;
        res.suspect = analyze(cA, nA);
        res.original = analyze(cB, nB);
        res.integrityMatch = (res.suspect.integrityHash == res.original.integrityHash);
        
        // Byte Similarity
        int dist = levenshteinDist(res.suspect.fuzzySig, res.original.fuzzySig);
        int maxLen = max(res.suspect.fuzzySig.length(), res.original.fuzzySig.length());
        res.similarity = (maxLen > 0) ? (1.0 - ((double)dist / (double)maxLen)) * 100.0 : 100.0;
        
        // NEW: Plagiarism Calculation
        res.plagiarismScore = calculatePlagiarism(cA, cB);

        stringstream ss;
        int len = (cA.size() < 512) ? cA.size() : 512;
        for (int i = 0; i < len; ++i) {
            ss << hex << setfill('0') << setw(2) << (int)(unsigned char)cA[i] << (i % 16 == 15 ? "\n" : " ");
        }
        res.hexDump = ss.str();
        return res;
    }
};

// ==========================================
// 4. HYPER-REALISTIC 3D UI GENERATOR
// ==========================================
class StealthUI {
public:
    static void generate(const CompareData& d) {
        ofstream f("CyberWatchdog_Warfare.html");
        
        f << "<!DOCTYPE html><html><head><title>OMEGA PROTOCOL</title>";
        f << "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>";
        f << "<script src='https://cdnjs.cloudflare.com/ajax/libs/vanilla-tilt/1.7.0/vanilla-tilt.min.js'></script>";
        f << "<style>";
        f << "@import url('https://fonts.googleapis.com/css2?family=Orbitron:wght@400;900&family=Rajdhani:wght@300;700&family=Share+Tech+Mono&display=swap');";
        
        f << ":root { --bg:#020205; --neon:#00f3ff; --alert:#ff003c; --warn:#ffae00; --text:#e0e0e0; --panel:rgba(10,15,25,0.7); --border:rgba(0,243,255,0.3); }";
        f << "body { margin:0; background:var(--bg); color:#fff; font-family:'Rajdhani', sans-serif; overflow-x:hidden; perspective: 1500px; }";
        f << ".scanlines { position: fixed; top: 0; left: 0; width: 100%; height: 100%; background: linear-gradient(to bottom, rgba(255,255,255,0), rgba(255,255,255,0) 50%, rgba(0,0,0,0.2) 50%, rgba(0,0,0,0.2)); background-size: 100% 4px; pointer-events: none; z-index: 999; animation: scanmove 0.5s linear infinite; opacity: 0.3; }";
        f << "#canvas-bg { position:fixed; top:0; left:0; width:100%; height:100%; z-index:-1; }";
        f << ".container { max-width:1600px; margin:0 auto; padding:40px; position:relative; z-index:1; transform-style: preserve-3d; opacity: 0; animation: bootUp 1.5s forwards ease-out; }";
        f << "@keyframes bootUp { 0% { opacity: 0; transform: translateY(50px) rotateX(10deg); } 100% { opacity: 1; transform: translateY(0) rotateX(0deg); } }";
        f << ".header { display:flex; justify-content:space-between; align-items:flex-end; border-bottom:1px solid var(--border); padding-bottom:30px; margin-bottom:50px; position:relative; }";
        f << ".header::after { content: ''; position: absolute; bottom: -2px; left: 0; width: 50%; height: 4px; background: var(--neon); box-shadow: 0 0 20px var(--neon); }";
        f << ".title { font-family:'Orbitron'; font-size:4em; font-weight:900; text-transform:uppercase; letter-spacing:8px; background: linear-gradient(90deg, #fff, var(--neon)); -webkit-background-clip: text; color: transparent; filter: drop-shadow(0 0 10px var(--neon)); }";
        f << ".dev-box { text-align:right; border-left:4px solid var(--neon); padding-left:20px; }";
        f << ".dev-name { font-size:1.8em; font-weight:900; color:#fff; letter-spacing:3px; display:block; text-shadow: 2px 2px 0px var(--alert), -2px -2px 0px var(--neon); animation: liquid 3s infinite alternate; }";
        f << "@keyframes liquid { 0% { text-shadow: 2px 2px 0px var(--alert), -2px -2px 0px var(--neon); } 100% { text-shadow: -2px -2px 0px var(--alert), 2px 2px 0px var(--neon); } }";
        
        f << ".grid { display:grid; grid-template-columns: repeat(4, 1fr); gap:40px; margin-bottom: 40px; }";
        f << ".card { background:var(--panel); border:1px solid var(--border); border-radius:15px; padding:30px; backdrop-filter: blur(20px); box-shadow: 0 20px 50px rgba(0,0,0,0.8), inset 0 0 0 rgba(255,255,255,0); transform-style: preserve-3d; transition: 0.5s; position:relative; overflow:hidden; }";
        f << ".card-label { font-size:0.9em; color:var(--neon); letter-spacing:4px; text-transform:uppercase; margin-bottom:15px; font-weight:bold; display: flex; align-items: center; gap: 10px; }";
        f << ".card-label::before { content:''; display:block; width:10px; height:10px; background:var(--neon); box-shadow: 0 0 10px var(--neon); border-radius:50%; }";
        f << ".card-val { font-family:'Orbitron'; font-size:3.5em; font-weight:900; transform: translateZ(60px); text-shadow: 0 10px 30px rgba(0,0,0,0.8); }";
        f << ".alert { border-color:var(--alert) !important; box-shadow: 0 0 30px rgba(255,0,60,0.2) !important; }";
        f << ".alert .card-label { color:var(--alert); } .alert .card-label::before { background:var(--alert); box-shadow: 0 0 10px var(--alert); }";
        f << ".alert .card-val { color:var(--alert); text-shadow: 0 0 20px var(--alert); }";
        f << ".table-container { grid-column: span 2; position: relative; }";
        f << "table { width:100%; border-collapse:collapse; margin-top:20px; font-size:0.9em; }";
        f << "th { color:var(--neon); text-align:left; padding:15px; font-family:'Orbitron'; letter-spacing:2px; border-bottom:1px solid #444; }";
        f << "td { padding:15px; border-bottom:1px solid #222; color:#ccc; background: rgba(0,0,0,0.2); transition: 0.3s; }";
        f << "tr:hover td { background: rgba(0,243,255,0.1); color: #fff; }";
        f << ".high-risk { color:var(--alert); font-weight:bold; text-shadow: 0 0 10px var(--alert); }";
        f << ".entropy-map { height:50px; display:flex; border-radius:5px; overflow:hidden; margin-top:20px; border:1px solid #333; box-shadow: 0 5px 15px rgba(0,0,0,0.5); }";
        f << ".zone { flex:1; display:flex; align-items:center; justify-content:center; font-weight:bold; font-size:0.8em; font-family:'Orbitron'; color:#000; transition: width 1s ease-in-out; position: relative; }";
        f << ".zone:hover { flex: 1.5; z-index: 10; box-shadow: 0 0 20px rgba(0,0,0,0.8); }";
        f << ".console { background:#080808; height:300px; padding:20px; font-family:'Share Tech Mono'; color:var(--neon); border:1px solid #333; box-shadow: inset 0 0 50px #000; font-size:0.9em; line-height:1.8; overflow: hidden; }";
        f << ".console div { border-left: 2px solid var(--neon); padding-left: 10px; margin-bottom: 5px; opacity: 0; animation: slideIn 0.3s forwards; }";
        f << "@keyframes slideIn { from { opacity: 0; transform: translateX(-20px); } to { opacity: 1; transform: translateX(0); } }";

        f << "</style></head><body><div class='scanlines'></div><canvas id='canvas-bg'></canvas>";
        f << "<div class='container'>";
        
        f << "<div class='header'><div class='title'>CYBER-WATCHDOG</div>";
        f << "<div class='dev-box'><span class='dev-label'>SYSTEM ARCHITECTS</span><span class='dev-name'>" << DEV_1 << "</span><span class='dev-name'>" << DEV_2 << "</span></div></div>";
        
        f << "<div class='grid'>";
        string tClass = (d.suspect.threatScore > 0) ? "alert" : "";
        f << "  <div class='card " << tClass << "' data-tilt data-tilt-glare><div class='card-label'>THREAT LEVEL</div><div class='card-val'>" << (int)d.suspect.threatScore << "%</div></div>";
        
        // --- NEW PLAGIARISM CARD ---
        string pClass = (d.plagiarismScore > 20.0) ? "alert" : "";
        f << "  <div class='card " << pClass << "' data-tilt data-tilt-glare><div class='card-label'>PLAGIARISM SCORE</div><div class='card-val'>" << (int)d.plagiarismScore << "%</div><div style='color:#888; font-size:0.7em; margin-top:10px; transform: translateZ(20px)'>CONTENT SIMILARITY</div></div>";

        string rClass = d.suspect.isRansomware ? "alert" : "";
        string rStat = d.suspect.isRansomware ? "DETECTED" : "CLEAN";
        f << "  <div class='card " << rClass << "' data-tilt data-tilt-glare><div class='card-label'>RANSOMWARE</div><div class='card-val' style='font-size:2em'>" << rStat << "</div>";
        if(d.suspect.isRansomware) f << "<div style='color:var(--alert); font-size:0.7em; margin-top:10px; font-weight:700; transform: translateZ(30px)'>REASON: " << d.suspect.ransomwareReason << "</div>";
        f << "  </div>";
        
        f << "  <div class='card' data-tilt data-tilt-glare><div class='card-label'>CAP-HASH</div><div class='card-val' style='font-size:1.5em; word-break:break-all; color:var(--neon)'>" << d.suspect.capHash << "</div></div>";
        
        f << "  <div class='card table-container'><div class='card-label'>THREAT INTELLIGENCE LOG</div>";
        if(d.suspect.detections.empty()) f << "<p style='text-align:center; padding:40px; color:#555; font-family:Orbitron'>NO ACTIVE THREATS SCANNED</p>";
        else {
            f << "<table><tr><th>SEVERITY</th><th>THREAT NAME</th><th>CATEGORY</th><th>DATA SOURCE</th></tr>";
            for(const auto& det : d.suspect.detections) {
                f << "<tr><td class='high-risk'>" << det.riskScore << "%</td><td>" << det.name << "</td><td>" << det.type << "</td><td style='font-family:monospace; color:#fff'>" << det.matchedData << "</td></tr>";
            }
            f << "</table>";
        }
        f << "  </div>";

        f << "  <div class='card' style='grid-column: span 2;'><div class='card-label'>VECTOR DIMENSIONS</div><canvas id='radar' style='max-height:300px'></canvas></div>";

        f << "  <div class='card' style='grid-column: span 2;'><div class='card-label'>ZONE ENTROPY DEPTH MAP</div>";
        f << "    <div class='entropy-map'>";
        auto getC = [](double e) { return (e > 7.0) ? "#ff003c" : (e > 5.5 ? "#ffae00" : "#00f3ff"); };
        f << "      <div class='zone' style='background:" << getC(d.suspect.zoneEnt.header) << "; color:#000'>HEAD: " << setprecision(2) << d.suspect.zoneEnt.header << "</div>";
        f << "      <div class='zone' style='background:" << getC(d.suspect.zoneEnt.code) << "; color:#000'>CODE: " << d.suspect.zoneEnt.code << "</div>";
        f << "      <div class='zone' style='background:" << getC(d.suspect.zoneEnt.data) << "; color:#000'>DATA: " << d.suspect.zoneEnt.data << "</div>";
        f << "      <div class='zone' style='background:" << getC(d.suspect.zoneEnt.overlay) << "; color:#000'>OVER: " << d.suspect.zoneEnt.overlay << "</div>";
        f << "    </div><p style='font-size:0.7em; color:#555; margin-top:15px; font-family:monospace'>Analysis of encrypted sections and overlay packers.</p></div>";

        f << "  <div class='card' style='grid-column: span 2;'><div class='card-label'>LIVE COMMAND BUFFER</div><div class='console' id='log'></div></div>";
        f << "</div></div>"; 
        
        f << "<script>";
        f << "VanillaTilt.init(document.querySelectorAll('.card'), { max: 15, speed: 400, perspective: 1000, scale: 1.02, glare: true, 'max-glare': 0.1 });";
        
        f << "new Chart(document.getElementById('radar'), { type:'radar', data: { labels:['Injection','Network','Crypto','Spyware','Plagiarism'], datasets:[{ label:'Threat Level', data:[";
        int inj=20, net=20, cry=20, spy=20;
        int plag = (int)d.plagiarismScore; // Use plagiarism score in chart
        for(const auto& b : d.suspect.behavior) {
            if(b.category=="Injection") inj=100; if(b.category=="Network") net=100; if(b.category=="Crypto") cry=100; if(b.category=="Keylogger") spy=100;
        }
        if(d.suspect.isRansomware) cry=100;
        f << inj << "," << net << "," << cry << "," << spy << "," << plag << "], backgroundColor:'rgba(0,243,255,0.2)', borderColor:'#00f3ff', borderWidth:2, pointBackgroundColor:'#fff', pointBorderColor:'#fff' }] }, options:{ scales:{ r:{ angleLines:{color:'#333'}, grid:{color:'#333'}, pointLabels:{color:'#00f3ff', font:{family:'Orbitron', size:12}}, ticks:{display:false}, suggestMin:0, suggestMax:100 } }, plugins:{legend:{display:false}} } });";

        f << "const log = document.getElementById('log'); const msgs = ['[System] OMEGA CORE ONLINE...', '[I/O] Analyzing Target: " << d.suspect.filename << "'];";
        for(const auto& b : d.suspect.behavior) f << "msgs.push('[Detect] MITRE " << b.mitrePhase << " -> " << b.apiName << "');";
        if(d.suspect.isRansomware) f << "msgs.push('[CRITICAL] RANSOMWARE LOGIC MAPPED');";
        f << "msgs.push('[Analysis] Plagiarism Index: " << d.plagiarismScore << "%');";
        f << "let i=0; function type(){ if(i<msgs.length){ log.innerHTML += `<div>> ${msgs[i]}</div>`; log.scrollTop=log.scrollHeight; i++; setTimeout(type, 150); } } type();";

        f << "const c = document.getElementById('canvas-bg'); const ctx = c.getContext('2d'); c.width=window.innerWidth; c.height=window.innerHeight;";
        f << "let hexs=[]; for(let i=0; i<50; i++) hexs.push({x:Math.random()*c.width, y:Math.random()*c.height, r:Math.random()*30+10, s:Math.random()*0.02, o:Math.random()});";
        f << "function draw(){ ctx.clearRect(0,0,c.width,c.height); ctx.lineWidth=1;";
        f << "hexs.forEach(h=>{ ctx.strokeStyle=`rgba(0,243,255,${h.o * 0.3})`; ctx.beginPath(); for(let j=0; j<6; j++){ ctx.lineTo(h.x + h.r*Math.cos(j*Math.PI/3 + h.s), h.y + h.r*Math.sin(j*Math.PI/3 + h.s)); } ctx.closePath(); ctx.stroke(); h.s+=0.005; h.o = Math.abs(Math.sin(Date.now() * 0.001 * h.r)); }); requestAnimationFrame(draw); } draw();";
        
        f << "</script></body></html>";
        f.close();
    }
};

// ==========================================
// 5. MAIN
// ==========================================
string readFile(const string& path) {
    ifstream file(path.c_str(), ios::binary | ios::ate);
    if (!file) return "";
    streamsize size = file.tellg();
    file.seekg(0, ios::beg);
    string buffer(size, '\0');
    if (file.read(&buffer[0], size)) return buffer;
    return "";
}

int main() {
    cout << "=======================================================" << endl;
    cout << "   CYBER-WATCHDOG: OMEGA PROTOCOL" << endl;
    cout << "   ARCHITECTS: " << DEV_1 << " & " << DEV_2 << endl;
    cout << "=======================================================" << endl;
    
    string fA, fB;
    cout << "\n[+] TARGET SUSPECT: "; getline(cin, fA);
    cout << "[+] TARGET BASELINE: "; getline(cin, fB);
    
    if(!fA.empty() && fA.front() == '"') fA = fA.substr(1, fA.length()-2);
    if(!fB.empty() && fB.front() == '"') fB = fB.substr(1, fB.length()-2);
    
    string cA = readFile(fA);
    string cB = readFile(fB);
    
    if(cA.empty() || cB.empty()) { cerr << "[!] DATA ACCESS DENIED OR EMPTY FILE." << endl; return 1; }
    
    cout << "[*] Running Deep-Field 3D Analysis & Plagiarism Check..." << endl;
    CompareData res = OmegaEngine::compare(cA, fA, cB, fB);
    StealthUI::generate(res);
    
    cout << "\n[SUCCESS] Reporting to: CyberWatchdog_Warfare.html" << endl;
    system("start CyberWatchdog_Warfare.html");
    
    return 0;
}

#include "tester.h"
#include "callCount.h"
#include "nodeConfig.h"

using namespace std;

int Manual(testLogger& log);
int Print(testLogger& log);
int PrintWide(testLogger& log);
int PrintWideMulti(testLogger& log);
int RegPrint(testLogger& log);
int InvalidRegex(testLogger& log);


int main(int argc, const char *argv[])
{
    Test("Manuallying adding calls...",Manual).RunTest();
    Test("Printing Results...",Print).RunTest();
    Test("Printing Wide Results...",PrintWide).RunTest();
    Test("Printing Wide Results...",PrintWideMulti).RunTest();
    Test("Printing Search Results...",RegPrint).RunTest();
    Test("Checking Invalid Regex",InvalidRegex).RunTest();
    return 0;
}

int Manual(testLogger& log) {
    NodeConfig::Instance().Reset();
    StringStructFactory& factory = NodeConfig::Instance().CostFactory();
    CallCount counter;
    StringStruct s100 = factory.New("100");
    StringStruct s200 = factory.New("200");
    StringStruct s101 = factory.New("101");
    StringStruct s500 = factory.New("500");
    counter.AddCall("Func1",s100);
    counter.AddCall("Func2",s200);
    counter.AddCall("Func1",s101);
    counter.AddCall("main",s500);

    // Now validate the times...
    auto main = counter.GetCount("main");
    auto f1 = counter.GetCount("Func1");
    auto f2 = counter.GetCount("Func2");
    auto unknown = counter.GetCount("XXX");

    if ( main.calls != 1 ) {
        log << "Invalid call count for main: " << main.calls;
        return 1;
    }
    if ( main[0] != 500 ) {
        log << "Invalid call time for main : " << main[0];
        return 1;
    }

    if ( f1.calls != 2 ) {
        log << "Invalid call count for f1: " << f1.calls;
        return 1;
    }
    if ( f1[0] != 201 ) {
        log << "Invalid call time for f1 : " << f1[0];
        return 1;
    }

    if ( f2.calls != 1 ) {
        log << "Invalid call count for f2: " << f2.calls;
        return 1;
    }
    if ( f2[0] != 200 ) {
        log << "Invalid call time for f2 : " << f2[0];
        return 1;
    }

    if ( unknown.calls != 0 ) {
        log << "Invalid call count for unknown: " << unknown.calls;
        return 1;
    }
    if ( unknown[0] != 0 ) {
        log << "Invalid call time for unknown : " << unknown[0];
        return 1;
    }

    return 0;
}

int Print(testLogger& log) {
    NodeConfig::Instance().Reset();
    StringStructFactory& factory = NodeConfig::Instance().CostFactory();
    CallCount counter;
    StringStruct s100 = factory.New("100");
    StringStruct s200 = factory.New("200");
    StringStruct s102 = factory.New("102");
    StringStruct s500 = factory.New("500");
    counter.AddCall("Func1",s100);
    counter.AddCall("Func2",s200);
    counter.AddCall("Func1",s102);
    counter.AddCall("Really Long Name that is far too long to fit in the box, no really it is really long",s500);

    string expected = 
"-----------------------------------------------------------------------------------------------------------\n"
"|-               Most Time Spent in Function                                                             -|\n"
"-----------------------------------------------------------------------------------------------------------\n"
"|    Function Name                                                      | Calls  | Time       | Time/call |\n"
"-----------------------------------------------------------------------------------------------------------\n"
"| Really Long Name that is far too long to fit in the box, no really ...| 1      | 500        | 500       |\n"
"| Func1                                                                 | 2      | 202        | 101       |\n"
"| Func2                                                                 | 1      | 200        | 200       |\n"
"-----------------------------------------------------------------------------------------------------------\n"
"\n"
"-----------------------------------------------------------------------------------------------------------\n"
"|-               Most Expensive Function Calls                                                           -|\n"
"-----------------------------------------------------------------------------------------------------------\n"
"|    Function Name                                                      | Calls  | Time       | Time/call |\n"
"-----------------------------------------------------------------------------------------------------------\n"
"| Really Long Name that is far too long to fit in the box, no really ...| 1      | 500        | 500       |\n"
"| Func2                                                                 | 1      | 200        | 200       |\n"
"| Func1                                                                 | 2      | 202        | 101       |\n"
"-----------------------------------------------------------------------------------------------------------\n";
    string expected_shorter = 
"-----------------------------------------------------------------------------------------------------------\n"
"|-               Most Time Spent in Function                                                             -|\n"
"-----------------------------------------------------------------------------------------------------------\n"
"|    Function Name                                                      | Calls  | Time       | Time/call |\n"
"-----------------------------------------------------------------------------------------------------------\n"
"| Really Long Name that is far too long to fit in the box, no really ...| 1      | 500        | 500       |\n"
"| Func1                                                                 | 2      | 202        | 101       |\n"
"-----------------------------------------------------------------------------------------------------------\n"
"\n"
"-----------------------------------------------------------------------------------------------------------\n"
"|-               Most Expensive Function Calls                                                           -|\n"
"-----------------------------------------------------------------------------------------------------------\n"
"|    Function Name                                                      | Calls  | Time       | Time/call |\n"
"-----------------------------------------------------------------------------------------------------------\n"
"| Really Long Name that is far too long to fit in the box, no really ...| 1      | 500        | 500       |\n"
"| Func2                                                                 | 1      | 200        | 200       |\n"
"-----------------------------------------------------------------------------------------------------------\n";
    string actual = counter.PrintResults();
    string shorter = counter.PrintResults(2); 

    if ( expected != actual ) {
        log << "Expected: " << endl << ">" << expected << "<";
        log << "Got: " << endl <<  ">" << actual << "<";
        return 1;
    }

    if ( shorter != expected_shorter ) {
        log << "Expected: " << endl << ">" << expected_shorter << "<";
        log << "Got: " << endl <<  ">" << shorter << "<";
        return 1;
    }
   return 0;
}

int PrintWide(testLogger& log) {
    NodeConfig::Instance().Reset();
    StringStructFactory& factory = NodeConfig::Instance().CostFactory();
    CallCount counter;
    StringStruct s100 = factory.New("100");
    StringStruct s200 = factory.New("200");
    StringStruct s102 = factory.New("102");
    StringStruct s500 = factory.New("500");
    counter.AddCall("Func1",s100);
    counter.AddCall("Func2",s200);
    counter.AddCall("Func1",s102);
    counter.AddCall("Really Long Name that is far too long to fit in the box, no really it is really long",s500);

    string expected = 
"                 Most Time Spent in Function\n"
"               ===============================\n"
" Calls    Time       Name\n"
"-------  ---------  -------\n"
"1        500        Really Long Name that is far too long to fit in the box, no really it is really long\n"
"2        202        Func1\n"
"1        200        Func2\n"
"\n"
"\n"
"                 Most Expensive Function Calls\n"
"               =================================\n"
" Calls    Av Time    Name\n"
"-------  ---------  -------\n"
"1        500        Really Long Name that is far too long to fit in the box, no really it is really long\n"
"1        200        Func2\n"
"2        101        Func1\n";
    string expected_shorter = 
"                 Most Time Spent in Function\n"
"               ===============================\n"
" Calls    Time       Name\n"
"-------  ---------  -------\n"
"1        500        Really Long Name that is far too long to fit in the box, no really it is really long\n"
"2        202        Func1\n"
"\n"
"\n"
"                 Most Expensive Function Calls\n"
"               =================================\n"
" Calls    Av Time    Name\n"
"-------  ---------  -------\n"
"1        500        Really Long Name that is far too long to fit in the box, no really it is really long\n"
"1        200        Func2\n";
    string actual = counter.WidePrint();
    string shorter = counter.WidePrint(2); 

    if ( expected != actual ) {
        log << "Expected: " << endl << ">" << expected << "<";
        log << "Got: " << endl <<  ">" << actual << "<";
        return 1;
    }

    if ( shorter != expected_shorter ) {
        log << "Expected: " << endl << ">" << expected_shorter << "<";
        log << "Got: " << endl <<  ">" << shorter << "<";
        return 1;
    }
   return 0;
}

int PrintWideMulti(testLogger& log) {
    NodeConfig::Instance().Reset();
    NodeConfig::Instance().ConfigureCostFactory("Ir Dr");
    for ( const auto& i : NodeConfig::Instance().DisplayIdxs() ) {
        log << i << endl;
    }
    StringStructFactory& factory = NodeConfig::Instance().CostFactory();
    CallCount counter;
    StringStruct s100 = factory.New("100 1");
    StringStruct s200 = factory.New("200 2");
    StringStruct s102 = factory.New("102 1");
    StringStruct s500 = factory.New("500 5");
    counter.AddCall("Func1",s100);
    counter.AddCall("Func2",s200);
    counter.AddCall("Func1",s102);
    counter.AddCall("Really Long Name that is far too long to fit in the box, no really it is really long",s500);

    string expected = 
"                 Most Time Spent in Function\n"
"               ===============================\n"
" Calls    Ir         Dr         Name\n"
"-------  ---------  ---------  -------\n"
"1        500        5          Really Long Name that is far too long to fit in the box, no really it is really long\n"
"2        202        2          Func1\n"
"1        200        2          Func2\n"
"\n"
"\n"
"                 Most Expensive Function Calls\n"
"               =================================\n"
" Calls    Av Ir      Av Dr      Name\n"
"-------  ---------  ---------  -------\n"
"1        500        5          Really Long Name that is far too long to fit in the box, no really it is really long\n"
"1        200        2          Func2\n"
"2        101        1          Func1\n";
    string expected_shorter = 
"                 Most Time Spent in Function\n"
"               ===============================\n"
" Calls    Ir         Dr         Name\n"
"-------  ---------  ---------  -------\n"
"1        500        5          Really Long Name that is far too long to fit in the box, no really it is really long\n"
"2        202        2          Func1\n"
"\n"
"\n"
"                 Most Expensive Function Calls\n"
"               =================================\n"
" Calls    Av Ir      Av Dr      Name\n"
"-------  ---------  ---------  -------\n"
"1        500        5          Really Long Name that is far too long to fit in the box, no really it is really long\n"
"1        200        2          Func2\n";
    string actual = counter.WidePrint();
    string shorter = counter.WidePrint(2); 

    if ( expected != actual ) {
        log << "Expected: " << endl << ">" << expected << "<";
        log << "Got: " << endl <<  ">" << actual << "<";
        return 1;
    }

    if ( shorter != expected_shorter ) {
        log << "Expected: " << endl << ">" << expected_shorter << "<";
        log << "Got: " << endl <<  ">" << shorter << "<";
        return 1;
    }
   return 0;
}

int RegPrint(testLogger& log) {
    NodeConfig::Instance().Reset();
    StringStructFactory& factory = NodeConfig::Instance().CostFactory();
    CallCount counter;
    StringStruct s100 = factory.New("100");
    StringStruct s200 = factory.New("200");
    StringStruct s102 = factory.New("102");
    StringStruct s500 = factory.New("500");
    counter.AddCall("Other1",s100);
    counter.AddCall("Other1",s100);
    counter.AddCall("Other3",s100);
    counter.AddCall("Other2",s100);
    counter.AddCall("Func1",s100);
    counter.AddCall("Func2",s200);
    counter.AddCall("Func1",s102);
    counter.AddCall("Really Long Name that is far too long to fit in the box, no really it is really long",s500);

    string expected = 
"                 Most Time Spent in Function\n"
"               ===============================\n"
" Calls    Time       Name\n"
"-------  ---------  -------\n"
"1        500        Really Long Name that is far too long to fit in the box, no really it is really long\n"
"2        202        Func1\n"
"1        200        Func2\n"
"\n"
"\n"
"                 Most Expensive Function Calls\n"
"               =================================\n"
" Calls    Av Time    Name\n"
"-------  ---------  -------\n"
"1        500        Really Long Name that is far too long to fit in the box, no really it is really long\n"
"1        200        Func2\n"
"2        101        Func1\n";
    string expected_shorter = 
"                 Most Time Spent in Function\n"
"               ===============================\n"
" Calls    Time       Name\n"
"-------  ---------  -------\n"
"1        500        Really Long Name that is far too long to fit in the box, no really it is really long\n"
"2        202        Func1\n"
"\n"
"\n"
"                 Most Expensive Function Calls\n"
"               =================================\n"
" Calls    Av Time    Name\n"
"-------  ---------  -------\n"
"1        500        Really Long Name that is far too long to fit in the box, no really it is really long\n"
"1        200        Func2\n";
    string actual = counter.FilteredPrint("(unc|eally)");
    string shorter = counter.FilteredPrint("(Func|Really).*",2); 

    if ( expected != actual ) {
        log << "Expected: " << endl << ">" << expected << "<";
        log << "Got: " << endl <<  ">" << actual << "<";
        return 1;
    }

    if ( shorter != expected_shorter ) {
        log << "Expected: " << endl << ">" << expected_shorter << "<";
        log << "Got: " << endl <<  ">" << shorter << "<";
        return 1;
    }
   return 0;
}

int InvalidRegex(testLogger& log) {
    NodeConfig::Instance().Reset();
    StringStructFactory& factory = NodeConfig::Instance().CostFactory();
    CallCount counter;
    StringStruct s100 = factory.New("100");
    StringStruct s200 = factory.New("200");
    StringStruct s102 = factory.New("102");
    StringStruct s500 = factory.New("500");
    counter.AddCall("Other1",s100);
    counter.AddCall("Other1",s100);
    counter.AddCall("Other3",s100);
    counter.AddCall("Other2",s100);
    counter.AddCall("Func1",s100);
    counter.AddCall("Func2",s200);
    counter.AddCall("Func1",s102);
    counter.AddCall("Really Long Name that is far too long to fit in the box, no really it is really long",s500);

    string expected = "Invalid regular expression: \n";
    string actual = counter.FilteredPrint("(Func|Really.*");
    if ( actual.find(expected) == string::npos ) {
        log << "Failed to handle invalid regex!" << endl;
        log << "Expected: " << expected << endl;
        log << "Actual: " << actual << endl;
        return 1;
    }
    return 0;
}

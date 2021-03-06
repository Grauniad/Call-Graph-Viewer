#include "tester.h"
#include "SourceFile.h"
#include <string>
#include "callgrindTree.h"
#include "nodeConfig.h"

using namespace std;

int basic(testLogger& log );
int annotate(testLogger& log );
int load_ids(testLogger& log);
int annotate_native(testLogger& log);

int main(int argc, const char *argv[])
{
    Test("Print some source...",basic).RunTest();
    Test("Annotating some source...",annotate).RunTest();
    Test("Checking file names...",load_ids).RunTest();
    Test("Annotating Source...",annotate_native).RunTest();
    return 0;
}

int basic (testLogger& log ) {
    SourceFile file("data/graph.cpp");
    string actual = file.Print(5,20);

    string expected = 
"    6: int RootNode(testLogger& log);\n"
"    7: int AddNodes(testLogger& log);\n"
"    8: int CheckResults(testLogger& log);\n"
"    9: int CheckShortResults(testLogger& log);\n"
"   10: int PathAccess(testLogger& log);\n"
"   11: int Search(testLogger& log);\n"
"   12: int CheckRegSearch(testLogger& log);\n"
"   13: int MakeNode(testLogger& log);\n"
"   14: int EmptyCallgrindData(testLogger& log);\n"
"   15: int EmptyCallgrindData_Native(testLogger& log);\n"
"   16: int CallgrindTree(testLogger& log);\n"
"   17: int CallgrindTree_Native(testLogger& log);\n"
"   18: int CallgrindTable(testLogger& log);\n"
"   19: int CallgrindTable_Native(testLogger& log);\n"
"   20: int LS(testLogger& log);\n";

    if ( actual != expected ) {
       log << "Invalid source file!" << endl;
       log << "Expected: " << expected <<  endl;
       log << "Actual: " << actual <<  endl;
        return 1;
    }
    return 0;
}

int annotate (testLogger& log ) {
    NodeConfig::Instance().Reset();
    SourceFile file("data/graph.cpp");
    Annotation a;
    StringStruct s34235 = NodeConfig::Instance().CostFactory().New("34235");
    StringStruct s342 = NodeConfig::Instance().CostFactory().New("342");
    StringStruct s999 = NodeConfig::Instance().CostFactory().New("999");
    a.AddAnnotation(9,s34235);
    a.AddAnnotation(10,s342);
    a.AddAnnotation(11,s999);
    string actual = file.Annotate(a,7,21);
    string expected = 
"    7:           : int RootNode(testLogger& log);\n"
"    8:           : int AddNodes(testLogger& log);\n"
"    9:      34235: int CheckResults(testLogger& log);\n"
"   10:        342: int CheckShortResults(testLogger& log);\n"
"   11:        999: int PathAccess(testLogger& log);\n"
"   12:           : int Search(testLogger& log);\n"
"   13:           : int CheckRegSearch(testLogger& log);\n"
"   14:           : int MakeNode(testLogger& log);\n"
"   15:           : int EmptyCallgrindData(testLogger& log);\n"
"   16:           : int EmptyCallgrindData_Native(testLogger& log);\n"
"   17:           : int CallgrindTree(testLogger& log);\n"
"   18:           : int CallgrindTree_Native(testLogger& log);\n"
"   19:           : int CallgrindTable(testLogger& log);\n"
"   20:           : int CallgrindTable_Native(testLogger& log);\n"
"   21:           : int LS(testLogger& log);\n";

    if ( actual != expected ) {
       log << "Invalid source file!" << endl;
       log << "Expected: >" << expected <<  "<" << endl;
       log << "Actual: " << ">" << actual <<  "<" << endl;
        return 1;
    }
    return 0;
}

int load_ids(testLogger& log ) {
    NodeConfig::Instance().Reset();
    CallgrindNative native("data/native/flist_files.callgrind");
    NodePtr div = native.RootNode()->GetNode(Path("main/evens/div"));

    if ( div.IsNull() ) {
        log << " Failed to build tree " << endl;
        log << native.RootNode()->PrintResults() << endl;
        return 1;
    }
    if ( div->SourceId() != 194 ) {
        log << "div doesn't live in 194!:  " << div->SourceId() << endl;
        return 1;
    }

    if ( native.GetFile(div->SourceId()).Name() != "data/graph2.cpp" ) {
        log << "Main doesn't live in the correct file:  >" << native.GetFile(div->SourceId()).Name() << "<" << endl;
        return 1;
    }
    return 0;
}

int annotate_native(testLogger& log ) {
    NodeConfig::Instance().Reset();
    CallgrindNative native("data/native/flist_files.callgrind");
    NodePtr div = native.RootNode()->GetNode(Path("main/evens"));

    if ( div->SourceStart() != 25 ) {
        log << " Invalid SourceStart: " << div->SourceStart() << endl;
        return 1;
    }

    string actual = native.Annotate(div,82);
    string expected = 
"   22:            ( 0%): \n"
"   23:            ( 0%): int main(int argc, const char *argv[])\n"
"   24:            ( 0%): {\n"
"   25:         64 !82%!: \n"
"   26:          7 ( 8%):     Test(\"Checking root node...\",RootNode).RunTest();\n"
"   27:            ( 0%):     Test(\"Adding some nodes...\",AddNodes).RunTest();\n"
"   28:            ( 0%):     Test(\"Checking path retrieval...\",PathAccess).RunTest();\n"
"   29:            ( 0%):     Test(\"Printing Results...\",CheckResults).RunTest();\n"
"   30:            ( 0%):     Test(\"Printing Short Results...\",CheckShortResults).RunTest();\n"
"   31:            ( 0%):     Test(\"Seaching the graph\",Search).RunTest();\n"
"   32:            ( 0%):     Test(\"Seaching the graph with regex\",CheckRegSearch).RunTest();\n"
"   33:            ( 0%):     Test(\"Testing child creation\",MakeNode).RunTest();\n"
"   34:            ( 0%):     Test(\"Loading an empty callggrind tree\",EmptyCallgrindData).RunTest();\n"
"   35:            ( 0%):     Test(\"Loading an empty callggrind tree from native file strings\",EmptyCal...\n"
"   36:            ( 0%):     Test(\"Loading a full callgrind tree\",CallgrindTree).RunTest();\n"
"   37:            ( 0%):     Test(\"Loading a full callgrind tree from native file strings\",CallgrindTr...\n"
"   38:            ( 0%):     Test(\"Loading a full callgrind tree and table\",CallgrindTable).RunTest();\n"
"   39:            ( 0%):     Test(\"Loading a full callgrind tree and table from native file strings\",C...\n"
"   40:          7 ( 8%):     Test(\"Tabulating Children (LS)...\",LS).RunTest();\n"
"   41:            ( 0%):     return 0;\n"
"   42:            ( 0%): }\n"
"   43:            ( 0%): \n"
"   44:            ( 0%): int CheckNode(testLogger& log,\n"
"   45:            ( 0%):               NodePtr node,\n"
"   46:            ( 0%):               bool isRoot,\n"
"   47:            ( 0%):               string name,\n";

    if (actual != expected ) {
        log << "Annotation: >" << native.Annotate(div) << "<" << endl;
        log << "Expected : >" << expected  << "<" << endl;
        return 1;
    }

    return 0;
}

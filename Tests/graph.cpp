#include "tester.h"
#include "node.h"
#include "nodeSearchCache.h"
#include "callgrindTree.h"
#include "nodeConfig.h"
#include "stringStruct.h"

int RootNode(testLogger& log);
int AddNodes(testLogger& log);
int AddNodesWithCosts(testLogger& log);
int CheckResults(testLogger& log);
int CheckResultsWithCosts(testLogger& log);
int CheckShortResults(testLogger& log);
int PathAccess(testLogger& log);
int Search(testLogger& log);
int SearchMaxDepth(testLogger& log);
int CheckRegSearch(testLogger& log);
int CheckLocalRegSearch(testLogger& log);
int MakeNode(testLogger& log);
int EmptyCallgrindData(testLogger& log);
int EmptyCallgrindData_Native(testLogger& log);
int CallgrindTree(testLogger& log);
int CallgrindTree_Native(testLogger& log);
int CallgrindTable(testLogger& log);
int CallgrindTable_Native(testLogger& log);
int LS(testLogger& log);

int main(int argc, const char *argv[])
{

    Test("Checking root node...",RootNode).RunTest();
    Test("Adding some nodes...",AddNodes).RunTest();
    Test("Adding some nodes with cost structs...",AddNodesWithCosts).RunTest();
    Test("Checking path retrieval...",PathAccess).RunTest();
    Test("Printing Results...",CheckResults).RunTest();
    Test("Printing Results for two units...",CheckResultsWithCosts).RunTest();
    Test("Printing Short Results...",CheckShortResults).RunTest();
    Test("Seaching the graph",Search).RunTest();
    Test("Seaching the graph with a limited depth",SearchMaxDepth).RunTest();
    Test("Seaching the graph with regex",CheckRegSearch).RunTest();
    Test("Seaching the local graph with regex",CheckLocalRegSearch).RunTest();
    Test("Testing child creation",MakeNode).RunTest();
    Test("Loading an empty callggrind tree",EmptyCallgrindData).RunTest();
    Test("Loading an empty callggrind tree from native file strings",EmptyCallgrindData_Native).RunTest();
    Test("Loading a full callgrind tree",CallgrindTree).RunTest();
    Test("Loading a full callgrind tree from native file strings",CallgrindTree_Native).RunTest();
    Test("Loading a full callgrind tree and table",CallgrindTable).RunTest();
    Test("Loading a full callgrind tree and table from native file strings",CallgrindTable_Native).RunTest();
    Test("Tabulating Children (LS)...",LS).RunTest();
    return 0;
}

int CheckNode(testLogger& log,
              NodePtr node,
              bool isRoot,
              string name,
              long   count,
              long   time,
              size_t children) 
{
    log << "Checking " << node->Name() << " against " << name << endl;
    if ( isRoot != node->IsRoot() ) {
        log << "Node has incorrect root setting" << endl;
        return 1;
    }

    if ( node->Name() != name ) {
        log << "Unexpected Name!" << endl;
        log << "Expected: " << name << endl;
        log << "Actual : " << node->Name() << endl;
        return 1;
    }

    if ( node->Calls() != count ) {
        log << "Unexpected Call Count!" << endl;
        log << "Expected: " << count << endl;
        log << "Actual : " << node->Calls() << endl;
        return 1;
    }

    if ( node->RunTime() != time ) {
        log << "Unepxect Run time!" << endl;
        log << "Expected: " << time << endl;
        log << "Actual : " << node->RunTime() << endl;
        return 1;
    }
    if  ( node->NumChildren() != children ) {
        log << "Unepxect ## of children!" << endl;
        log << "Expected: " << children << endl;
        log << "Actual : " << node->NumChildren() << endl;
        return 1;
    }
    return 0;
}

int CheckNode(testLogger& log,
              NodePtr node,
              bool isRoot,
              string name,
              long   count,
              long   Ir,
              long   Dr,
              size_t children) 
{
    log << "Checking " << node->Name() << " against " << name << endl;
    if ( isRoot != node->IsRoot() ) {
        log << "Node has incorrect root setting" << endl;
        return 1;
    }

    if ( node->Name() != name ) {
        log << "Unexpected Name!" << endl;
        log << "Expected: " << name << endl;
        log << "Actual : " << node->Name() << endl;
        return 1;
    }

    if ( node->Calls() != count ) {
        log << "Unexpected Call Count!" << endl;
        log << "Expected: " << count << endl;
        log << "Actual : " << node->Calls() << endl;
        return 1;
    }

    if ( node->Costs()[0] != Ir ) {
        log << "Unepxect Ir!" << endl;
        log << "Expected: " << Ir << endl;
        log << "Actual : " << node->Costs()[0] << endl;
        return 1;
    }

    if ( node->Costs()[1] != Dr ) {
        log << "Unepxect Dr!" << endl;
        log << "Expected: " << Dr << endl;
        log << "Actual : " << node->Costs()[1] << endl;
        return 1;
    }
    if  ( node->NumChildren() != children ) {
        log << "Unepxect ## of children!" << endl;
        log << "Expected: " << children << endl;
        log << "Actual : " << node->NumChildren() << endl;
        return 1;
    }
    return 0;
}

int RootNode(testLogger& log) {
    Node rootNode;

    if  ( !rootNode.Parent().IsNull() ) {
        log << "Root's parent is not null!" << endl;
        return 1;
    }

    return CheckNode(log,rootNode.THIS(),true,  // isRoot
                                  "ROOT", // name
                                  0,      // count
                                  0,      // time
                                  0);     // children
}

int AddNodes(testLogger& log) {
    Node rootNode;
    Path rootPath("");
    Path mainPath("main");
    Path f1path("main/f1");

    /*
     * ROOT
     * |
     * main---F1 (2x101)---F2 (2x102)--NULL
     *      |            |
     *      |            --F3 (1x103)--NULL
     *      --F3 (2x103)
     */

    // Add F2 and F3 to F1
    NodePtr f2 = rootNode.AddCall(f1path.Root(),"f2",102);
    rootNode.AddCall(f1path.Root(),"f2",102);
    NodePtr f3_f1 = rootNode.AddCall(f1path.Root(),"f3",103);

    // Add F3 to main 
    NodePtr f3_main = rootNode.AddCall(mainPath.Root(),"f3",103);
    rootNode.AddCall(mainPath.Root(),"f3",103);

    // Add F1 to main (again)
    rootNode.AddCall(mainPath.Root(),"f1",101);

    // Add F1 to main
    NodePtr f1 = rootNode.AddCall(mainPath.Root(),"f1",101);

    // Add main to root
    NodePtr main = rootNode.AddCall(rootPath.Root(),"main",100);


    int ok = CheckNode(log,main, false,  // isRoot
                                 "main", // name
                                 1,      // count
                                 100,    // time
                                 2);     // children
    if ( ok == 0 ) {
        ok = CheckNode(log,f1, false,  // isRoot
                               "f1", // name
                               2,      // count
                               202,    // time
                               2);     // children
    }

    if ( ok == 0 ) {
        ok = CheckNode(log,f2, false,  // isRoot
                               "f2", // name
                               2,      // count
                               204,    // time
                               0);     // children
    }

    if ( ok == 0 ) {
        ok = CheckNode(log,f3_f1, false,  // isRoot
                               "f3", // name
                               1,      // count
                               103,    // time
                               0);     // children
    }

    if ( ok == 0 ) {
        ok = CheckNode(log,f3_main, false,  // isRoot
                               "f3", // name
                               2,      // count
                               206,    // time
                               0);     // children
    }

    /*
     * Finally check the root node...
     */
    if ( ok == 0 ) {
        ok = CheckNode(log,main->Parent(), 
                               true,   // isRoot
                               "ROOT", // name
                               0,      // count
                               0,      // time
                               1);     // children
    }

    if ( ok == 0 ) {
        ok = CheckNode(log,rootNode.THIS(), 
                               true,   // isRoot
                               "ROOT", // name
                               0,      // count
                               0,      // time
                               1);     // children
    }
    return ok;
}

int AddNodesWithCosts(testLogger& log ) {
    NodeConfig::Instance().Reset();
    NodeConfig::Instance().ConfigureCostFactory("Ir Dr");

    Node rootNode;
    Path rootPath("");
    Path mainPath("main");
    Path f1path("main/f1");


    /*
     * ROOT
     * |
     * main---F1 (2x101,5)---F2 (2x102,2)--NULL
     *      |            |
     *      |            --F3 (1x103,0)--NULL
     *      --F3 (2x103,8)
     */
    StringStructFactory& factory = NodeConfig::Instance().CostFactory();

    // Call F2 twice from F1...
    NodePtr f2 = rootNode.AddCall(f1path.Root(),"f2",factory.New("102 2"));
    rootNode.AddCall(f1path.Root(),"f2",factory.New("102 2"));

    // Call F3 once from F1...
    NodePtr f3_f1 = rootNode.AddCall(f1path.Root(),"f3",factory.New("103"));

    // Add F3 to main (twice)...
    NodePtr f3_main = rootNode.AddCall(mainPath.Root(),"f3",factory.New("103 8"));
    rootNode.AddCall(mainPath.Root(),"f3",factory.New("103 8"));

    // Add F1 to main, twice...
    rootNode.AddCall(mainPath.Root(),"f1",factory.New("101 5"));
    NodePtr f1 = rootNode.AddCall(mainPath.Root(),"f1",factory.New("101 5"));

    // Add main to root
    NodePtr main = rootNode.AddCall(rootPath.Root(),"main",100);

    int ok = CheckNode(log,main, false,  // isRoot
                                 "main", // name
                                 1,      // count
                                 100,    // Ir
                                 0,      // Dr
                                 2);     // children
    if ( ok == 0 ) {
        ok = CheckNode(log,f1, false,  // isRoot
                               "f1",   // name
                               2,      // count
                               202,    // Ir
                               10,     // Dr
                               2);     // children
    }

    if ( ok == 0 ) {
        ok = CheckNode(log,f2, false,  // isRoot
                               "f2", // name
                               2,      // count
                               204,    // Ir
                               4,      // Dr
                               0);     // children
    }

    if ( ok == 0 ) {
        ok = CheckNode(log,f3_f1, false,  // isRoot
                               "f3",      // name
                               1,         // count
                               103,       // Ir
                               0,       // Dr
                               0);        // children
    }

    if ( ok == 0 ) {
        ok = CheckNode(log,f3_main, false,  // isRoot
                               "f3",        // name
                               2,           // count
                               206,         // Ir
                               16,           // Dr
                               0);          // children
    }

    /*
     * Finally check the root node...
     */
    if ( ok == 0 ) {
        ok = CheckNode(log,main->Parent(), 
                               true,   // isRoot
                               "ROOT", // name
                               0,      // count
                               0,      // Ir
                               0,      // Dr
                               1);     // children
    }

    if ( ok == 0 ) {
        ok = CheckNode(log,rootNode.THIS(), 
                               true,   // isRoot
                               "ROOT", // name
                               0,      // count
                               0,      // Ir
                               0,      // Dr
                               1);     // children
    }
    return ok;
}

int CheckResults(testLogger& log) {
    NodeConfig::Instance().Reset();
    Node rootNode;
    Path rootPath("");

    Path mainPath("main");
    Path f1path("main/f1");

    /*
     * ROOT
     * |
     * main---F1 (2x101)---F2 (2x102)--NULL
     *      |            |
     *      |            --F3 (1x103)--NULL
     *      --F3 (2x103)
     */

    // Add main to root
    rootNode.AddCall(rootPath.Root(),"main",100);

    // Add F1 to main 
    rootNode.AddCall(mainPath.Root(),"f1",101); 

    // Add F2 and F3 to F1
    rootNode.AddCall(f1path.Root(),"f2",102);
    rootNode.AddCall(f1path.Root(),"f2",102);
    rootNode.AddCall(f1path.Root(),"f3",103);

    // Add F3 to main 
    rootNode.AddCall(mainPath.Root(),"f3",103);
    rootNode.AddCall(mainPath.Root(),"f3",103);

    // Add F1 to main (again)
    rootNode.AddCall(mainPath.Root(),"f1",101);

    string actual = rootNode.PrintResults();
    string expected = 
       "ROOT (ROOT)\n"
       "    Calls: 0, Time: 0, Av. Time: 0\n"
       "    main (ROOT/main)\n"
       "        Calls: 1, Time: 100, Av. Time: 100\n"
       "        f3 (ROOT/main/f3)\n"
       "            Calls: 2, Time: 206, Av. Time: 103\n"
       "        f1 (ROOT/main/f1)\n"
       "            Calls: 2, Time: 202, Av. Time: 101\n"
       "            f2 (ROOT/main/f1/f2)\n"
       "                Calls: 2, Time: 204, Av. Time: 102\n"
       "            f3 (ROOT/main/f1/f3)\n"
       "                Calls: 1, Time: 103, Av. Time: 103\n";

    if ( expected != actual ) {
       log << expected <<endl;
       log << "------------------------" << endl;
       log << actual <<endl;
       return 1;
    }

    return 0;
}

int CheckResultsWithCosts(testLogger& log) {
    NodeConfig::Instance().Reset();
    NodeConfig::Instance().ConfigureCostFactory("Ir Dr");
    Node rootNode;
    Path rootPath("");

    Path mainPath("main");
    Path f1path("main/f1");

    /*
     * ROOT
     * |
     * main---F1 (2x101,5)---F2 (2x102,2)--NULL
     *      |            |
     *      |            --F3 (1x103,0)--NULL
     *      --F3 (2x103,8)
     */

    StringStructFactory& factory = NodeConfig::Instance().CostFactory();

    // Call F2 twice from F1...
    rootNode.AddCall(f1path.Root(),"f2",factory.New("102 2"));
    rootNode.AddCall(f1path.Root(),"f2",factory.New("102 2"));

    // Call F3 once from F1...
    rootNode.AddCall(f1path.Root(),"f3",factory.New("103"));

    // Add F3 to main (twice)...
    rootNode.AddCall(mainPath.Root(),"f3",factory.New("103 8"));
    rootNode.AddCall(mainPath.Root(),"f3",factory.New("103 8"));

    // Add F1 to main, twice...
    rootNode.AddCall(mainPath.Root(),"f1",factory.New("101 5"));
    rootNode.AddCall(mainPath.Root(),"f1",factory.New("101 5"));

    // Add main to root
    rootNode.AddCall(rootPath.Root(),"main",100);

    string actual = rootNode.PrintResults();
    string expected = 
       "ROOT (ROOT)\n"
       "    Calls: 0, Ir: 0, Av. Ir: 0\n"
       "    Calls: 0, Dr: 0, Av. Dr: 0\n"
       "    main (ROOT/main)\n"
       "        Calls: 1, Ir: 100, Av. Ir: 100\n"
       "        Calls: 1, Dr: 0, Av. Dr: 0\n"
       "        f3 (ROOT/main/f3)\n"
       "            Calls: 2, Ir: 206, Av. Ir: 103\n"
       "            Calls: 2, Dr: 16, Av. Dr: 8\n"
       "        f1 (ROOT/main/f1)\n"
       "            Calls: 2, Ir: 202, Av. Ir: 101\n"
       "            Calls: 2, Dr: 10, Av. Dr: 5\n"
       "            f2 (ROOT/main/f1/f2)\n"
       "                Calls: 2, Ir: 204, Av. Ir: 102\n"
       "                Calls: 2, Dr: 4, Av. Dr: 2\n"
       "            f3 (ROOT/main/f1/f3)\n"
       "                Calls: 1, Ir: 103, Av. Ir: 103\n"
       "                Calls: 1, Dr: 0, Av. Dr: 0\n";

    if ( expected != actual ) {
       log << expected <<endl;
       log << "------------------------" << endl;
       log << actual <<endl;
       return 1;
    }

    /*
     * Now switch to only printing Dr
     * (NOTE: we haven't changed the sort yet...)
     */ 
    NodeConfig::Instance().ConfigureDisplay("Dr");
    expected = 
       "ROOT (ROOT)\n"
       "    Calls: 0, Dr: 0, Av. Dr: 0\n"
       "    main (ROOT/main)\n"
       "        Calls: 1, Dr: 0, Av. Dr: 0\n"
       "        f3 (ROOT/main/f3)\n"
       "            Calls: 2, Dr: 16, Av. Dr: 8\n"
       "        f1 (ROOT/main/f1)\n"
       "            Calls: 2, Dr: 10, Av. Dr: 5\n"
       "            f2 (ROOT/main/f1/f2)\n"
       "                Calls: 2, Dr: 4, Av. Dr: 2\n"
       "            f3 (ROOT/main/f1/f3)\n"
       "                Calls: 1, Dr: 0, Av. Dr: 0\n";
    actual = rootNode.PrintResults();
    if ( expected != actual ) {
       log << expected <<endl;
       log << "---------(Dr) ---------------" << endl;
       log << actual <<endl;
       return 1;
    }

    /*
     * Now print both, in reverse order...
     * (NOTE: we haven't changed the sort yet...)
     */ 
    NodeConfig::Instance().ConfigureDisplay("Dr Ir");

    actual = rootNode.PrintResults();
    expected = 
       "ROOT (ROOT)\n"
       "    Calls: 0, Dr: 0, Av. Dr: 0\n"
       "    Calls: 0, Ir: 0, Av. Ir: 0\n"
       "    main (ROOT/main)\n"
       "        Calls: 1, Dr: 0, Av. Dr: 0\n"
       "        Calls: 1, Ir: 100, Av. Ir: 100\n"
       "        f3 (ROOT/main/f3)\n"
       "            Calls: 2, Dr: 16, Av. Dr: 8\n"
       "            Calls: 2, Ir: 206, Av. Ir: 103\n"
       "        f1 (ROOT/main/f1)\n"
       "            Calls: 2, Dr: 10, Av. Dr: 5\n"
       "            Calls: 2, Ir: 202, Av. Ir: 101\n"
       "            f2 (ROOT/main/f1/f2)\n"
       "                Calls: 2, Dr: 4, Av. Dr: 2\n"
       "                Calls: 2, Ir: 204, Av. Ir: 102\n"
       "            f3 (ROOT/main/f1/f3)\n"
       "                Calls: 1, Dr: 0, Av. Dr: 0\n"
       "                Calls: 1, Ir: 103, Av. Ir: 103\n";

    if ( expected != actual ) {
       log << expected <<endl;
       log << "---------(Dr Ir) ---------------" << endl;
       log << actual <<endl;
       return 1;
    }

    return 0;
}

int CheckShortResults(testLogger& log) {
    NodeConfig::Instance().Reset();
    Node rootNode;
    Path rootPath("");

    Path mainPath("main");
    Path f1path("main/f1");

    /*
     * ROOT
     * |
     * main---F1 (2x101)---F2 (2x102)--NULL
     *      |            |
     *      |            --F3 (1x103)--NULL
     *      --F3 (2x103)
     */

    // Add main to root
    NodePtr mainNode = rootNode.AddCall(rootPath.Root(),"main",100);

    // Add F1 to main 
    rootNode.AddCall(mainPath.Root(),"f1",101); 

    // Add F2 and F3 to F1
    rootNode.AddCall(f1path.Root(),"f2",102);
    rootNode.AddCall(f1path.Root(),"f2",102);
    rootNode.AddCall(f1path.Root(),"f3",103);

    // Add F3 to main 
    rootNode.AddCall(mainPath.Root(),"f3",103);
    rootNode.AddCall(mainPath.Root(),"f3",103);

    // Add F1 to main (again)
    rootNode.AddCall(mainPath.Root(),"f1",101);

    string actual = mainNode->PrintResults(0,1);
    string expected = 
       "main (main)\n"
       "    Calls: 1, Time: 100, Av. Time: 100\n"
       "    f3 (main/f3)\n"
       "        Calls: 2, Time: 206, Av. Time: 103\n"
       "    f1 (main/f1)\n"
       "        Calls: 2, Time: 202, Av. Time: 101\n";

    if ( expected != actual ) {
       log << expected <<endl;
       log << "------------------------" << endl;
       log << actual <<endl;
       return 1;
    }

    return 0;
}

int PathAccess(testLogger& log) {
    NodeConfig::Instance().Reset();
    Node rootNode;
    Path rootPath("");

    Path mainPath("main");
    Path f1path("main/f1");

    /*
     * ROOT
     * |
     * main---F1 (2x101)---F2 (2x102)--NULL
     *      |            |
     *      |            --F3 (1x103)--NULL
     *      --F3 (2x103)
     */

    // Add main to root
    rootNode.AddCall(rootPath.Root(),"main",100);

    // Add F1 to main 
    rootNode.AddCall(mainPath.Root(),"f1",101); 

    // Add F2 and F3 to F1
    rootNode.AddCall(f1path.Root(),"f2",102);
    rootNode.AddCall(f1path.Root(),"f2",102);
    rootNode.AddCall(f1path.Root(),"f3",103);

    // Add F3 to main 
    rootNode.AddCall(mainPath.Root(),"f3",103);
    rootNode.AddCall(mainPath.Root(),"f3",103);

    // Add F1 to main (again)
    rootNode.AddCall(mainPath.Root(),"f1",101);

    Path fakePath("main/f1/f2/notHere");

    NodePtr f1 = rootNode.GetNode(f1path);

    if ( f1.IsNull() ) {
        log << "f1 is a null node!" << endl;
        return 1;
    }

    int ok = CheckNode(log,f1, false,  // isRoot
                               "f1", // name
                               2,      // count
                               202,    // time
                               2);     // children
    return ok;
}

int Search(testLogger& log) {
    NodeConfig::Instance().Reset();
    Node rootNode;
    Path rootPath("");

    Path mainPath("main");
    Path f1path("main/f1");

    /*
     * ROOT
     * |
     * main---F1 (2x101)---F2 (2x102)--NULL
     *      |            |
     *      |            --F3 (1x103)--NULL
     *      --F3 (2x103)
     */

    // Add main to root
    rootNode.AddCall(rootPath.Root(),"main",100);

    // Add F1 to main 
    rootNode.AddCall(mainPath.Root(),"f1",101); 

    // Add F2 and F3 to F1
    rootNode.AddCall(f1path.Root(),"f2",102);
    rootNode.AddCall(f1path.Root(),"f2",102);
    rootNode.AddCall(f1path.Root(),"f3",103);

    // Add F3 to main 
    rootNode.AddCall(mainPath.Root(),"f3",103);
    rootNode.AddCall(mainPath.Root(),"f3",103);

    // Add F1 to main (again)
    rootNode.AddCall(mainPath.Root(),"f1",101);

    SearchCache cache;
    NodePtr rootPtr = rootNode.THIS();

    cache.AddTree(rootPtr);

    SearchResult result = cache.Search("XXX");

    if ( result.Ok() ) {
        log << "Search returned ok from a stupid search! " << endl;
        return 1;
    }

    if ( result.Remaining() != 0 ) {
        log << "Search returned non-zero from a stupid search! " << endl;
        log << result.Remaining();
        return 1;
    }

    if ( !result.Node().IsNull() ) {
        log << "Result is not null from a stupid search!" << endl;
        return 1;
    }

    SearchResult f3_result = cache.Search("f3");

    if ( f3_result.Remaining() != 1 ) {
        log << "Expected only one more result!" << endl;
        log << f3_result.Remaining();
        return 1;
    }

    if ( f3_result.Node()->Parent()->Name() != "main" ) {
        log << "I thought the frist result would be main :(" << endl;
        log << f3_result.Node()->Parent()->Name() << endl;
        return 1;
    }

    if ( f3_result.Next().Node()->Parent()->Name() != "f1" ) {
        log << "I thought the second result would be f1 :(" << endl;
        log << f3_result.Node()->Parent()->Name() << endl;
        return 1;
    }

    if ( f3_result.Next().Previous().Node()->Parent()->Name() != "main" ) {
        log << "It doesn't semm to live back!" << endl;
        log << f3_result.Next().Previous().Node()->Parent()->Name() << endl;
        return 1;
    }

    ++f3_result;

    if ( f3_result.Node()->Parent()->Name() != "f1" ) {
        log << "I thought the second result would be f1 :(" << endl;
        log << f3_result.Node()->Parent()->Name() << endl;
        return 1;
    }

    if ( f3_result.Remaining() != 0 ) {
        log << "Search returned non-zero when at the end of the search!" << endl;
        log << f3_result.Remaining();
        return 1;
    }

    if ( !f3_result.Ok() ) {
        log << "Search not ok on the last element!" << endl;
        return 1;
    }

    --f3_result;

    if ( f3_result.Remaining() != 1 ) {
        log << "--Expected only one more result!" << endl;
        log << f3_result.Remaining();
        return 1;
    }

    if ( f3_result.Node()->Parent()->Name() != "main" ) {
        log << "--I thought the frist result would be main :(" << endl;
        log << f3_result.Node()->Parent()->Name() << endl;
        return 1;
    }

    if ( f3_result.Next().Node()->Parent()->Name() != "f1" ) {
        log << "--I thought the second result would be f1 :(" << endl;
        log << f3_result.Node()->Parent()->Name() << endl;
        return 1;
    }

    ++f3_result;
    ++f3_result;

    if ( f3_result.Remaining() != 0 ) {
        log << "Search returned non-zero when past the end of a search" << endl;
        log << f3_result.Remaining();
        return 1;
    }

    if ( f3_result.Ok() ) {
        log << "Search returned ok when past the end of a search" << endl;
        return 1;
    }


    if ( !f3_result.Node().IsNull() ) {
        log << "Result is not null when past the end of the search!" << endl;
        return 1;
    }

    return 0;
}

int SearchMaxDepth(testLogger& log) {
    Node rootNode;
    Path rootPath("");

    Path mainPath("main");
    Path f1path("main/f1");

    /*
     * ROOT
     * |
     * main---F1 (2x101)---F2 (2x102)--NULL
     *      |            |
     *      |            --F3 (1x103)--NULL
     *      --F3 (2x103)
     */

    // Add main to root
    rootNode.AddCall(rootPath.Root(),"main",100);

    // Add F1 to main
    rootNode.AddCall(mainPath.Root(),"f1",101);

    // Add F2 and F3 to F1
    rootNode.AddCall(f1path.Root(),"f2",102);
    rootNode.AddCall(f1path.Root(),"f2",102);
    rootNode.AddCall(f1path.Root(),"f3",103);

    // Add F3 to main
    rootNode.AddCall(mainPath.Root(),"f3",103);
    rootNode.AddCall(mainPath.Root(),"f3",103);

    // Add F1 to main (again)
    rootNode.AddCall(mainPath.Root(),"f1",101);

    SearchCache cache;
    NodePtr rootPtr = rootNode.THIS();

    cache.AddTree(rootPtr,2);

    SearchResult result = cache.Search("XXX");

    if ( result.Ok() ) {
        log << "Search returned ok from a stupid search! " << endl;
        return 1;
    }

    if ( result.Remaining() != 0 ) {
        log << "Search returned non-zero from a stupid search! " << endl;
        log << result.Remaining();
        return 1;
    }

    if ( !result.Node().IsNull() ) {
        log << "Result is not null from a stupid search!" << endl;
        return 1;
    }

    SearchResult f3_result = cache.Search("f3");

    if ( f3_result.Remaining() != 0 ) {
        log << "Expected only no more results!" << endl;
        log << f3_result.Remaining();
        return 1;
    }

    if ( f3_result.Node()->Parent()->Name() != "main" ) {
        log << "I thought the frist result would be main :(" << endl;
        log << f3_result.Node()->Parent()->Name() << endl;
        return 1;
    }

    ++f3_result;

    if ( f3_result.Remaining() != 0 ) {
        log << "Search returned non-zero when past the end of a search" << endl;
        log << f3_result.Remaining();
        return 1;
    }

    if ( f3_result.Ok() ) {
        log << "Search returned ok when past the end of a search" << endl;
        return 1;
    }


    if ( !f3_result.Node().IsNull() ) {
        log << "Result is not null when past the end of the search!" << endl;
        return 1;
    }

    return 0;
}

int MakeNode (testLogger& log ) {
    NodeConfig::Instance().Reset();
    Node rootNode;
    NodePtr apple = rootNode.MakeChild("apple");
    NodePtr apple2 = rootNode.MakeChild("apple");

    if ( apple.IsNull() || apple2.IsNull() )  {
        log << "MakeChild returned null!" << endl;
        return 1;
    }

    if ( apple->Name() != "apple" || apple2->Name() != "apple" ) {
        log << "MakeChild did not add the correct ndoe!" << endl;
        return 1;
    }

    if ( rootNode.NumChildren() != 1 ) {
        log << "MakeCHild did not create exactly one child" << endl;
    }

    NodePtr pair = apple->Parent()->MakeChild("pair");

    if ( pair.IsNull() || pair->Name() != "pair" ) {
        log << "Make child did not add pair! " << endl;
        return 1;
    }

    if ( rootNode.NumChildren() != 2 ) {
        log << "MakeChild did not create a new child!" << endl;
    }


    return 0;
}

int EmptyCallgrindData ( testLogger& log ) {
    NodeConfig::Instance().Reset();
    CallgrindCallTree data("data/flist.csv");
    string expected = 
        "ROOT (ROOT)\n"
        "    Calls: 0, Time: 0, Av. Time: 0\n"
        "    main (ROOT/main)\n"
        "        Calls: 0, Time: 0, Av. Time: 0\n"
        "        odds (ROOT/main/odds)\n"
        "            Calls: 0, Time: 0, Av. Time: 0\n"
        "            div (ROOT/main/odds/div)\n"
        "                Calls: 0, Time: 0, Av. Time: 0\n"
        "        evens (ROOT/main/evens)\n"
        "            Calls: 0, Time: 0, Av. Time: 0\n"
        "            div (ROOT/main/evens/div)\n"
        "                Calls: 0, Time: 0, Av. Time: 0\n"
        "                pos_div2 (ROOT/main/evens/div/pos_div2)\n"
        "                    Calls: 0, Time: 0, Av. Time: 0\n"
        "                pos_div4 (ROOT/main/evens/div/pos_div4)\n"
        "                    Calls: 0, Time: 0, Av. Time: 0\n";

    string actual = data.RootNode()->PrintResults();

    if ( expected != actual ) {
        log << " Failed to read callgrind tree!";
        log << " Expected: " << endl;
        log << ">" << expected << "<" << endl;
        log << " Actual: " << endl;
        log << ">" << actual << "<" ;
        return 1;
    }
    return 0;
}

int CallgrindTree ( testLogger& log ) {
    NodeConfig::Instance().Reset();
    NodeConfig::Instance().ConfigureCostFactory("Ir");
    CallgrindCallTree data("data/flist.csv");
    data.LoadCalls("data/calls.csv");
    string expected = 
        "ROOT (ROOT)\n" 
        "    Calls: 0, Ir: 0, Av. Ir: 0\n" 
        "    main (ROOT/main)\n" 
        "        Calls: 0, Ir: 0, Av. Ir: 0\n" 
        "        odds (ROOT/main/odds)\n" 
        "            Calls: 1, Ir: 75, Av. Ir: 75\n" 
        "            div (ROOT/main/odds/div)\n" 
        "                Calls: 3, Ir: 45, Av. Ir: 15\n" 
        "        evens (ROOT/main/evens)\n" 
        "            Calls: 1, Ir: 65, Av. Ir: 65\n" 
        "            div (ROOT/main/evens/div)\n" 
        "                Calls: 2, Ir: 42, Av. Ir: 21\n" 
        "                pos_div2 (ROOT/main/evens/div/pos_div2)\n" 
        "                    Calls: 1, Ir: 7, Av. Ir: 7\n" 
        "                pos_div4 (ROOT/main/evens/div/pos_div4)\n" 
        "                    Calls: 1, Ir: 7, Av. Ir: 7\n";
    string actual = data.RootNode()->PrintResults();

    if ( expected != actual ) {
        log << " Failed to read callgrind tree!";
        log << " Expected: " << endl;
        log << ">" << expected << "<" << endl;
        log << " Actual: " << endl;
        log << ">" << actual << "<" ;
        return 1;
    }
    return 0;
}

int CallgrindTable ( testLogger& log ) {
    NodeConfig::Instance().Reset();
    NodeConfig::Instance().Reset();
    CallgrindCallTree data("data/flist.csv");
    data.LoadCalls("data/calls.csv");
    data.LoadCosts("data/cost.csv");
    string expected = 
"-----------------------------------------------------------------------------------------------------------\n"
"|-               Most Time Spent in Function                                                             -|\n"
"-----------------------------------------------------------------------------------------------------------\n"
"|    Function Name                                                      | Calls  | Time       | Time/call |\n"
"-----------------------------------------------------------------------------------------------------------\n"
"| main                                                                  | 0      | 152        | 0         |\n"
"| div                                                                   | 5      | 87         | 17        |\n"
"| odds                                                                  | 1      | 75         | 75        |\n"
"| evens                                                                 | 1      | 65         | 65        |\n"
"| pos_div2                                                              | 1      | 7          | 7         |\n"
"| pos_div4                                                              | 1      | 7          | 7         |\n"
"-----------------------------------------------------------------------------------------------------------\n"
"\n"
"-----------------------------------------------------------------------------------------------------------\n"
"|-               Most Expensive Function Calls                                                           -|\n"
"-----------------------------------------------------------------------------------------------------------\n"
"|    Function Name                                                      | Calls  | Time       | Time/call |\n"
"-----------------------------------------------------------------------------------------------------------\n"
"| odds                                                                  | 1      | 75         | 75        |\n"
"| evens                                                                 | 1      | 65         | 65        |\n"
"| div                                                                   | 5      | 87         | 17        |\n"
"| pos_div2                                                              | 1      | 7          | 7         |\n"
"| pos_div4                                                              | 1      | 7          | 7         |\n"
"| main                                                                  | 0      | 152        | 0         |\n"
"-----------------------------------------------------------------------------------------------------------\n";
    string actual = data.Counter().PrintResults();

    if ( expected != actual ) {
        log << " Failed to read callgrind tree!";
        log << " Expected: " << endl;
        log << ">" << expected << "<" << endl;
        log << " Actual: " << endl;
        log << ">" << actual << "<" ;
        return 1;
    }
    return 0;
}

int CheckLocalRegSearch(testLogger& log) {
    NodeConfig::Instance().Reset();
    Node rootNode;
    Path rootPath("");

    Path mainPath("main");
    Path f1path("main/f1");

    /*
     * ROOT
     * |
     * main---F1 (2x101)---F2 (2x102)--NULL
     *      |            |
     *      |            --F3 (1x103)--NULL
     *      --F3 (2x103)
     */

    // Add main to root
    rootNode.AddCall(rootPath.Root(),"main",100);

    // Add F1 to main 
    rootNode.AddCall(mainPath.Root(),"f1",101); 

    // Add F2 and F3 to F1
    rootNode.AddCall(f1path.Root(),"f2",102);
    rootNode.AddCall(f1path.Root(),"f2",102);
    rootNode.AddCall(f1path.Root(),"f3",103);

    // Add F3 to main 
    rootNode.AddCall(mainPath.Root(),"f3",103);
    rootNode.AddCall(mainPath.Root(),"f3",103);

    // Add F1 to main (again)
    rootNode.AddCall(mainPath.Root(),"f1",101);

    NodePtr rootPtr = rootNode.THIS();

    RegSearch finder;
    finder.Search(rootPtr, "f3",2);
    SearchResult result = finder.Results();

    if ( result.Remaining() != 0 ) {
        log << "Search returned an invlaid # of results (expected 0 remaining)" << endl;
        log << result.Remaining();
        return 1;
    }

    if ( result.Node().IsNull() ) {
        log << "Result node is null!" << endl;
        return 1;
    }

    if ( result.Node()->Parent()->Name() != "main" ) {
        log << "I thought the frist result would be in main :(" << endl;
        log << result.Node()->Parent()->Name() << endl;
        return 1;
    }

    return 0;
}

int CheckRegSearch(testLogger& log) {
    NodeConfig::Instance().Reset();
    Node rootNode;
    Path rootPath("");

    Path mainPath("main");
    Path f1path("main/f1");

    /*
     * ROOT
     * |
     * main---F1 (2x101)---F2 (2x102)--NULL
     *      |            |
     *      |            --F3 (1x103)--NULL
     *      --F3 (2x103)
     */

    // Add main to root
    rootNode.AddCall(rootPath.Root(),"main",100);

    // Add F1 to main 
    rootNode.AddCall(mainPath.Root(),"f1",101); 

    // Add F2 and F3 to F1
    rootNode.AddCall(f1path.Root(),"f2",102);
    rootNode.AddCall(f1path.Root(),"f2",102);
    rootNode.AddCall(f1path.Root(),"f3",103);

    // Add F3 to main 
    rootNode.AddCall(mainPath.Root(),"f3",103);
    rootNode.AddCall(mainPath.Root(),"f3",103);

    // Add F1 to main (again)
    rootNode.AddCall(mainPath.Root(),"f1",101);

    NodePtr rootPtr = rootNode.THIS();

    RegSearch finder;
    finder.Search(rootPtr, "(X|.*");
    SearchResult result = finder.Results();

    if ( result.Ok() ) {
        log << "Search returned ok from a stupid search! " << endl;
        return 1;
    }

    if ( result.Remaining() != 0 ) {
        log << "Search returned non-zero from a stupid search! " << endl;
        log << result.Remaining();
        return 1;
    }

    if ( !result.Node().IsNull() ) {
        log << "Result is not null from a stupid search!" << endl;
        return 1;
    }

    finder.Search(rootPtr,"^f3$");
    SearchResult f3_result = finder.Results();

    if ( f3_result.Remaining() != 1 ) {
        log << "Expected only one more result!" << endl;
        log << f3_result.Remaining();
        return 1;
    }

    if ( f3_result.Node()->Parent()->Name() != "main" ) {
        log << "I thought the frist result would be main :(" << endl;
        log << f3_result.Node()->Parent()->Name() << endl;
        return 1;
    }

    if ( f3_result.Next().Node()->Parent()->Name() != "f1" ) {
        log << "I thought the second result would be f1 :(" << endl;
        log << f3_result.Node()->Parent()->Name() << endl;
        return 1;
    }

    if ( f3_result.Next().Previous().Node()->Parent()->Name() != "main" ) {
        log << "It doesn't semm to live back!" << endl;
        log << f3_result.Next().Previous().Node()->Parent()->Name() << endl;
        return 1;
    }

    ++f3_result;

    if ( f3_result.Node()->Parent()->Name() != "f1" ) {
        log << "I thought the second result would be f1 :(" << endl;
        log << f3_result.Node()->Parent()->Name() << endl;
        return 1;
    }

    if ( f3_result.Remaining() != 0 ) {
        log << "Search returned non-zero when at the end of the search!" << endl;
        log << f3_result.Remaining();
        return 1;
    }

    if ( !f3_result.Ok() ) {
        log << "Search not ok on the last element!" << endl;
        return 1;
    }

    --f3_result;

    if ( f3_result.Remaining() != 1 ) {
        log << "--Expected only one more result!" << endl;
        log << f3_result.Remaining();
        return 1;
    }

    if ( f3_result.Node()->Parent()->Name() != "main" ) {
        log << "--I thought the frist result would be main :(" << endl;
        log << f3_result.Node()->Parent()->Name() << endl;
        return 1;
    }

    if ( f3_result.Next().Node()->Parent()->Name() != "f1" ) {
        log << "--I thought the second result would be f1 :(" << endl;
        log << f3_result.Node()->Parent()->Name() << endl;
        return 1;
    }

    ++f3_result;
    ++f3_result;

    if ( f3_result.Remaining() != 0 ) {
        log << "Search returned non-zero when past the end of a search" << endl;
        log << f3_result.Remaining();
        return 1;
    }

    if ( f3_result.Ok() ) {
        log << "Search returned ok when past the end of a search" << endl;
        return 1;
    }


    if ( !f3_result.Node().IsNull() ) {
        log << "Result is not null when past the end of the search!" << endl;
        return 1;
    }


    return 0;
}

int LS(testLogger& log) {
    NodeConfig::Instance().Reset();
    Node rootNode;
    Path rootPath("");

    Path mainPath("main");
    Path f1path("main/f1");

    /*
     * ROOT
     * |
     * main---F1 (3x101)---F2 (2x102)--NULL
     *      |            |
     *      |            --F3 (1x103)--NULL
     *      --F3 (2x103)
     */

    // Add main to root
    NodePtr mainNode = rootNode.AddCall(rootPath.Root(),"main",100);

    // Add F1 to main 
    rootNode.AddCall(mainPath.Root(),"f1",101); 

    // Add F2 and F3 to F1
    rootNode.AddCall(f1path.Root(),"f2",102);
    rootNode.AddCall(f1path.Root(),"f2",102);
    rootNode.AddCall(f1path.Root(),"f3",103);

    // Add F3 to main 
    rootNode.AddCall(mainPath.Root(),"f3",103);
    rootNode.AddCall(mainPath.Root(),"f3",103);

    // Add F1 to main (again)
    rootNode.AddCall(mainPath.Root(),"f1",101);
    rootNode.AddCall(mainPath.Root(),"f1",101);

    string actual = mainNode->Tabulate();
    string actual2 = mainNode->Tabulate(2);
    string expected2 = 
        "main\n"
        "    Calls: 1, Time: 100, Av. Time: 100\n"
        "\n"
        "\n"
        "                 Most Time Spent in Function\n"
        "               ===============================\n"
        " Calls    Time       Name\n"
        "-------  ---------  -------\n"
        "3        309        f3\n"
        "3        303        f1\n"
        "2        204        f2\n"
        "\n"
        "\n"
        "                 Most Expensive Function Calls\n"
        "               =================================\n"
        " Calls    Av Time    Name\n"
        "-------  ---------  -------\n"
        "3        103        f3\n"
        "2        102        f2\n"
        "3        101        f1\n";

    string expected = 
        "main\n"
        "    Calls: 1, Time: 100, Av. Time: 100\n"
        "\n"
        "\n"
        "                 Most Time Spent in Function\n"
        "               ===============================\n"
        " Calls    Time       Name\n"
        "-------  ---------  -------\n"
        "3        303        f1\n"
        "2        206        f3\n"
        "\n"
        "\n"
        "                 Most Expensive Function Calls\n"
        "               =================================\n"
        " Calls    Av Time    Name\n"
        "-------  ---------  -------\n"
        "2        103        f3\n"
        "3        101        f1\n";
    if ( expected != actual ) {
       log << "ls 1 failed! " << endl;
       log << ">" << expected << "<" << endl;
       log << "------------------------" << endl;
       log << ">" << actual << "<" << endl;
       return 1;
    }

    if ( expected2 != actual2 ) {
       log << mainNode->PrintResults() << endl;
       log << "ls 2 failed! " << endl;
       log << expected2 <<endl;
       log << "------------------------" << endl;
       log << actual2 <<endl;
       return 1;
    }


    return 0;
}

int EmptyCallgrindData_Native(testLogger& log) {
    NodeConfig::Instance().Reset();
    NodeConfig::Instance().ConfigureCostFactory("Ir");
    CallgrindNative native("data/native/flist.callgrind");

    string expected = 
        "ROOT (ROOT)\n"
        "    Calls: 0, Ir: 0, Av. Ir: 0\n"
        "    main (ROOT/main)\n"
        "        Calls: 0, Ir: 0, Av. Ir: 0\n"
        "        odds (ROOT/main/odds)\n"
        "            Calls: 0, Ir: 0, Av. Ir: 0\n"
        "            div (ROOT/main/odds/div)\n"
        "                Calls: 0, Ir: 0, Av. Ir: 0\n"
        "        evens (ROOT/main/evens)\n"
        "            Calls: 0, Ir: 0, Av. Ir: 0\n"
        "            div (ROOT/main/evens/div)\n"
        "                Calls: 0, Ir: 0, Av. Ir: 0\n"
        "                pos_div2 (ROOT/main/evens/div/pos_div2)\n"
        "                    Calls: 0, Ir: 0, Av. Ir: 0\n"
        "                pos_div4 (ROOT/main/evens/div/pos_div4)\n"
        "                    Calls: 0, Ir: 0, Av. Ir: 0\n";

    string actual = native.RootNode()->PrintResults();

    if ( expected != actual ) {
        log << " Failed to read callgrind tree!";
        log << " Expected: " << endl;
        log << ">" << expected << "<" << endl;
        log << " Actual: " << endl;
        log << ">" << actual << "<" ;
        return 1;
    }
    return 0;
}

int CallgrindTree_Native(testLogger& log) {
    NodeConfig::Instance().Reset();
    CallgrindNative native("data/native/flist_costs.callgrind");

    string expected = 
        "ROOT (ROOT)\n" 
        "    Calls: 0, Ir: 0, Av. Ir: 0\n" 
        "    main (ROOT/main)\n" 
        "        Calls: 1, Ir: 54, Av. Ir: 54\n" 
        "        odds (ROOT/main/odds)\n" 
        "            Calls: 1, Ir: 75, Av. Ir: 75\n" 
        "            div (ROOT/main/odds/div)\n" 
        "                Calls: 3, Ir: 45, Av. Ir: 15\n" 
        "        evens (ROOT/main/evens)\n" 
        "            Calls: 1, Ir: 65, Av. Ir: 65\n" 
        "            div (ROOT/main/evens/div)\n" 
        "                Calls: 2, Ir: 42, Av. Ir: 21\n" 
        "                pos_div2 (ROOT/main/evens/div/pos_div2)\n" 
        "                    Calls: 1, Ir: 7, Av. Ir: 7\n" 
        "                pos_div4 (ROOT/main/evens/div/pos_div4)\n" 
        "                    Calls: 1, Ir: 7, Av. Ir: 7\n";
    string actual = native.RootNode()->PrintResults();

    if ( expected != actual ) {
        log << " Failed to read callgrind tree!";
        log << " Expected: " << endl;
        log << ">" << expected << "<" << endl;
        log << " Actual: " << endl;
        log << ">" << actual << "<" ;
        return 1;
    }
    return 0;
}

int CallgrindTable_Native ( testLogger& log ) {
    NodeConfig::Instance().Reset();
    CallgrindNative native("data/native/flist_costs.callgrind");
    string expected = 
"-----------------------------------------------------------------------------------------------------------\n"
"|-               Most Time Spent in Function                                                             -|\n"
"-----------------------------------------------------------------------------------------------------------\n"
"|    Function Name                                                      | Calls  | Ir         | Ir/call   |\n"
"-----------------------------------------------------------------------------------------------------------\n"
"| div                                                                   | 5      | 87         | 17        |\n"
"| odds                                                                  | 1      | 75         | 75        |\n"
"| evens                                                                 | 1      | 65         | 65        |\n"
"| main                                                                  | 1      | 54         | 54        |\n"
"| pos_div2                                                              | 1      | 7          | 7         |\n"
"| pos_div4                                                              | 1      | 7          | 7         |\n"
"-----------------------------------------------------------------------------------------------------------\n"
"\n"
"-----------------------------------------------------------------------------------------------------------\n"
"|-               Most Expensive Function Calls                                                           -|\n"
"-----------------------------------------------------------------------------------------------------------\n"
"|    Function Name                                                      | Calls  | Ir         | Ir/call   |\n"
"-----------------------------------------------------------------------------------------------------------\n"
"| odds                                                                  | 1      | 75         | 75        |\n"
"| evens                                                                 | 1      | 65         | 65        |\n"
"| main                                                                  | 1      | 54         | 54        |\n"
"| div                                                                   | 5      | 87         | 17        |\n"
"| pos_div2                                                              | 1      | 7          | 7         |\n"
"| pos_div4                                                              | 1      | 7          | 7         |\n"
"-----------------------------------------------------------------------------------------------------------\n";
    string actual = native.Counter().PrintResults();

    if ( expected != actual ) {
        log << " Failed to read callgrind tree!";
        log << " Expected: " << endl;
        log << ">" << expected << "<" << endl;
        log << " Actual: " << endl;
        log << ">" << actual << "<" ;
        return 1;
    }
    return 0;
}

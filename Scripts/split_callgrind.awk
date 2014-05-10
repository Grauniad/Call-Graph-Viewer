# Process a callgrind ouput file
# (C) Luke Humphreys 2014
#
# Split a single callgrind file into 3 files:
#    flist.csv: Maps function ID to its name and path in the call graph
#      NOTE: A function may appear several times with different IDs if
#            it is called from different points in the code.
#    cost.csv:  The cost of each funtion ID
#    calls.csv: Each enty represents one function calling another, and
#               the cost of that call
#
#
# Building the function list:
#   o The callgrind format defines functions on the fly, (when it
#      first mentions it)
#   o We scan for these definitions (fn / cfn) and an entry when we
#     find them
#   o Assuming callgrind was run with --separat-callers=N the call
#   stack will be provided in the func name, separated by ':
#        func2'func1'main
#
# Building the flat profile:
#    o Each block in the callgrind file is a single function. Each line
#      in the block is either a header, or has the form:
#          <location info> <cost>
#    o The total cost is the sum of all of these lines.
#
#  Building the calls file:
#    o Calls are a stadnard cost list in a function block which have
#      been preceeded by header rows defining the call.
#    o When we process a cost line we check if the previous lines
#      defined this meta data, and if it did record the call

################################################
##        HELPER FUNCTIONS - FILES            ##
################################################

# Map a file to an ID
function DefineFile(id,name) {
    files[id] = name;
}

# Change the current file, to an already mapped file
function SetCurrentFile(id) {
    file = id;
    child_file = id;
}

################################################
##        HELPER FUNCTIONS - FUNC. BLOCKS     ##
################################################

# Set the current function, to an already mapped fn
function SetCurrentFunction(id) {
    fn = id;
}

# Enter a new function
#
#  o map the id
#  o Calculate the path back to main
#  o If the func was called by the main frame, report it
function DefineFunction(id,name) {
    _name = "";
    _path = "";
    _found_main = 0;
    split(name,_tokens,"'");
    for ( t in _tokens ) {

        if ( _tokens[t] == "main" ) {
            _found_main = 1;
        }
        if ( _tokens[t] == "(below" ) {
           # We don't handle the "(below main)" section...
            break;
        }

        if ( t == 1 ) {
            _name = _tokens[t];
        } else if ( t==2 ) {
            _path = _tokens[t];
        } else {
            _path = _tokens[t]  "/" _path;
        }
    }
    if (  _found_main == 1 ) {
        printf "%d,\"%s\",\"%s\"\n", id, _name, _path > "flist.csv"
    }
    funcs[id] = name;
    f_under_main[id] = _found_main;
}

# Add cost to the current block
function AddCost(cost ) {
    f_cost+= cost;
}

# Report the current function, then reset counters
function LeaveF() {
    if ( f_under_main[fn] ) {
        printf "%d,%d\n", fn, f_cost > "cost.csv"
    }
    ResetFInfo();
}

# Reset func. info
function ResetFInfo () {
    file = "";  
    fn = "";
    f_cost = 0;
}

################################################
##        HELPER FUNCTIONS - CALL HANDLING    ##
################################################

# Set the ID of the next function to be called by this block
function SetNextCalledFn(id) {
    child_fn = id;
}

# Set the ID of file where the next called function can be found...
function SetNextCalledFile(id) {
    child_file = id;
}

# Set the number of call made to the next child function in this block
function SetNumCalls(count) {
    child_calls = count;
}

# Do the actual call 
function MakeCall(cost) {
    if ( child_fn != -1  && f_under_main[fn] && f_under_main[child_fn] ) 
    {
        printf "%d,%d,%d,%d\n", fn,child_fn,child_calls,cost > "calls.csv"
        ResetChildInfo();
    }
    # else: this is a cost in the current fn, don't care 
}

function ResetChildInfo() {
    child_calls = 0;
    child_file = fn;
    child_fn = -1;
}


################################################
##        INITIALISATION                      ##
################################################

BEGIN {
    # Setup to split fields on ' ', '=' or parentheses
    FS=" |=|=\(|\) |\)";

    # ID Maps
    files[-1] = "NO SUCH FILE!"
    funcs[-1] = "NO SUCH FUNCTION!"
    f_under_main[-1] = 0

    ResetFInfo();
    ResetChildInfo();

}

################################################
##        BLOCK HEADERS                       ##
################################################
#
# Read at the start of a function definition
#     fl: The file this function is found in
#     fn: The name of this function


##############
## NEW FILE ## 
##############
#    e.g fl=(1) file1.c
/^fl=\([0-9]*\) .+/ {
    id = $2;
    name = $3;

    DefineFile(id,name);
    SetCurrentFile(id);
}

###################
## EXISTING FILE ## 
###################
#    e.g fl=(1) file1.c
/^fl=\([0-9]*\)$/ {
    id = $2;
    SetCurrentFile(id);
}

#################
# NEW FUNCTION ##
#################
#   e.g fn=(1) main
/^fn=\([0-9]*\) .+/ {
    id = $2;
    name = $3;

    DefineFunction(id,name);
    SetCurrentFunction(id);
}

######################
# EXISTING FUNCTION ##
######################
#   e.g fn=(1) main
/^fn=\([0-9]*\)$/ {
    id = $2;
    SetCurrentFunction(id);
}

################################################
##        CHILD HEADERS                       ##
################################################
#
# Found inside BLOCKs. They set up a function call
#     cfn: The name of the child function 
#     cfi: The file this function is found in
#     calls=<num> <rel pos> The number of calls

##############
## NEW FILE ## 
##############
#    e.g fl=(1) file1.c
/^cfi=\([0-9]*\) .+/ {
    id = $2;
    name = $3;

    DefineFile(id,name);
    SetNextCalledFile(id);
}

###################
## EXISTING FILE ## 
###################
#    e.g fl=(1) file1.c
/^cfi=\([0-9]*\)$/ {
    id = $2;

    SetNextCalledFile(id);
}

#################
# NEW FUNCTION ##
#################
#   e.g cfn=(2)
/^cfn=\([0-9]*\) .+/ {
    id = $2;
    name = $3;

    DefineFunction(id,name);
    SetNextCalledFn(id);
}

######################
# EXISTING FUNCTION ##
######################
#   e.g cfn=(2)
/^cfn=\([0-9]*\)$/ {
    id = $2;
    SetNextCalledFn(id);
}

#################
# SET CALLS    ##
#################
#   e.g calls=1 50
/calls=.*/ {
    calls=$2
    SetNumCalls(calls);
}

################################################
##        STANDARD ROWS                       ##
################################################

################
## COST LINES ##
################
#    e.g 16 400
/^(\*|\+|-|[0-9])+ [0-9]+$/ {
    cost=$2

    MakeCall(cost)
    AddCost(cost)
}

#########################
## LEAVE CURRENT BLOCK ##
#########################
#  (a blank line)
/^$/ {
    LeaveF();
}
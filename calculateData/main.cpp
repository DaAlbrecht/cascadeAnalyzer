#include <string>
#include <iostream>
#include <fstream>
#include <utility>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/property_map/transform_value_property_map.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/connected_components.hpp>

using namespace std;
using namespace boost;

class Block;
class Connection;

struct BlockProperty
{
    Block *block_ptr;
};

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, BlockProperty> NetworkGraph;
typedef std::vector<Block*> BlockArray;
typedef std::vector<Connection*> ConnectionArray;
typedef std::vector <boost::graph_traits<NetworkGraph>::vertex_descriptor> vertex_desc;

bool ReadInputData(const std::string &filename, BlockArray &blockArray, ConnectionArray &connectionArray, NetworkGraph &g);

bool parseBlockLine(const std::string &line, BlockArray &blockArray, ConnectionArray &connectionArray, NetworkGraph &g);
void parseNetLine(const std::string &line, BlockArray &blockArray, ConnectionArray &connectionArray);
void parseGainBlock(std::vector<std::string> &buf, BlockArray &blockArray, ConnectionArray &connectionArray, NetworkGraph &g);
void parseMultiPortBlock(std::vector<std::string> &buf, BlockArray &blockArray, ConnectionArray &connectionArray, NetworkGraph &g);

bool drcParsing(BlockArray &blockArray, ConnectionArray &connectionArray);
bool CheckIfNetsAreUnique(BlockArray &blockArray, ConnectionArray &connectionArray);
bool CheckConnectionToMaxNumberOfDriverPorts(BlockArray &blockArray, ConnectionArray &connectionArray);
bool CheckConnectionToMaxNumberOfReceiverPorts(BlockArray &blockArray, ConnectionArray &connectionArray);

void CreateGraph(BlockArray &blockArray, ConnectionArray &connectionArray, NetworkGraph &g);
bool GraphContainsNoCycles(NetworkGraph &g);
bool Checkconnected_components(NetworkGraph &g);
vertex_desc GetSortedVertexDesctiptors(BlockArray &blockArray, NetworkGraph &g); // name in domain; block vs vertex
void CalculateGain(BlockArray &blockArray, NetworkGraph &g, vertex_desc &vertexdescriptors_swap);
void CalculateNoiseFigure(BlockArray &blockArray, NetworkGraph &g, vertex_desc &vertexdescriptors_swap);

bool drcGraph(NetworkGraph &g);

void CreateOutputFile(const std::string &filename, BlockArray &blockArray, NetworkGraph &g, vertex_desc &vertexdescriptors_swap);

class Connection
{
public:
    Connection(std::string &nameDriver, uint8_t numberDriver, std::string &nameReceiver, uint8_t numberReceiver):
        nameDriver_(nameDriver), numberDriver_(numberDriver), nameReceiver_(nameReceiver), numberReceiver_(numberReceiver){}

    ~Connection(){}
    const std::string &getNameDriver()const{return nameDriver_;}
    uint8_t getNumberDriver()const{return numberDriver_;}
    const std::string &getNameReceiver()const{return nameReceiver_;}
    uint8_t getNumberReceiver()const{return numberReceiver_;}
private:
    std::string nameDriver_;
    uint8_t numberDriver_;
    std::string nameReceiver_;
    uint8_t numberReceiver_;
};

class Block
{
public:
    virtual double getElementGain()const = 0;
    double &getTotalGain() {return totalgain_;}
    virtual double getBlockNF()const = 0;
    double &getTotalNF() {return totalnf_;}
    const std::string &getName()const{return name_;}
    virtual uint8_t getNumOfInputs()const = 0;
    virtual uint8_t getNumOfOutputs()const = 0;
    boost::graph_traits<NetworkGraph>::vertex_descriptor descriptor_;
private:
    std::string name_;
    double totalgain_;
    double totalnf_;
protected:
    Block(const std::string &name):name_(name){}
    virtual ~Block(){}
};

class GainBlock: public Block
{
public:
    GainBlock(const std::string &name, double gain = 0.0, double nf = 0.0):
        Block(name),
        gain_(gain),
        nf_(nf)
    {}
    double getElementGain()const override
    {
        return gain_;
    }
    double getBlockNF()const override
    {
        return nf_;
    }

    uint8_t getNumOfInputs()const override{return 1;}
    uint8_t getNumOfOutputs()const override{return 1;}
private:
    double gain_;
    double nf_;
};

class MultiPortBlock : public Block
{
public:
    MultiPortBlock(const std::string &name, double gain = 0.0, double nf = 0.0, uint8_t outputs = 1, uint8_t inputs = 1):
        Block(name),
        gain_(gain),
        nf_(nf),
        outputs_(outputs),
        inputs_(inputs)
    {}
    double getElementGain()const override
    {
        return gain_;
    }
    double getBlockNF()const override
    {
        return nf_;
    }

    uint8_t getNumOfInputs()const override{ return outputs_;}
    uint8_t getNumOfOutputs()const override{ return inputs_;}
private:
    double gain_;
    double nf_;
    uint8_t outputs_;
    uint8_t inputs_;

};

typedef boost::graph_traits<NetworkGraph>::edge_descriptor edge_descriptor;

std::vector <boost::graph_traits<NetworkGraph>::vertex_descriptor> vertexdescriptors;


struct has_cycle { };

struct cycle_detector : public default_dfs_visitor
{
   void back_edge(edge_descriptor, const NetworkGraph &g)
   {
        throw has_cycle();
   }
};

struct addvertex_iterator {};

class getCorrectOrderOfDescriptors : public default_dfs_visitor
{
public:
    getCorrectOrderOfDescriptors(std::vector <boost::graph_traits<NetworkGraph>::vertex_descriptor> &vertexdescriptors):
        vertexdescriptors_(vertexdescriptors)
    {}

    template <class vertex, class Graph>
    void finish_vertex(vertex u, Graph &g) const
    {
        vertexdescriptors_.push_back(u);
    }
private:
    std::vector <boost::graph_traits<NetworkGraph>::vertex_descriptor> &vertexdescriptors_;
};

int main(int argc, const char** argv)
{
    if(argc != 3)
    {
        std::cerr << "Usage:" << argv[0] << " <input file name> <output file name>";
        return -1;
    }

    BlockArray blockArray;
    ConnectionArray connectionArray;
    NetworkGraph g;

    const string inpFilename = argv[1];
    const string outFilename = argv[2];
    if(!ReadInputData(argv[1], blockArray, connectionArray, g))
    {
        std::cout << "error parsing input file!\n";
        return -2;
    }

    if(!drcParsing(blockArray, connectionArray))
    {
        cout << "drc failed" << endl;
        return -3;
    }

    for(auto b : blockArray)
    {
        if(dynamic_cast<GainBlock*>(b))
            std::cout << "name: " << b->getName() << "\tg = " << b->getElementGain() << "\t NF = " << b->getBlockNF() << "\n";
        else if(dynamic_cast<MultiPortBlock*>(b))
            std::cout << "name: " << b->getName() << "\toutputs = " << b->getNumOfOutputs() << "\t inputs = " << b->getNumOfInputs() << "\n";
    }

    for(auto c : connectionArray)
    {
       std::cout << "name driver:  " << c->getNameDriver() << "\t" <<"number driver " << std::to_string(c->getNumberDriver()) << "\n";
       std::cout << "name Receiver:" << c->getNameReceiver() << "\t" <<"number Receiver " << std::to_string(c->getNumberReceiver()) << "\n";
    }

    CreateGraph(blockArray, connectionArray, g);

    if(!drcGraph(g))
    {
        cout << "drc Graph failed" << endl;
        return -4;
    }

    vertex_desc vertexDesc = GetSortedVertexDesctiptors(blockArray, g); /// name?

    CalculateGain(blockArray, g, vertexDesc);
    CalculateNoiseFigure(blockArray, g, vertexDesc);
    CreateOutputFile(outFilename, blockArray, g, vertexDesc);

    return 0;
}

bool ReadInputData(const std::string &filename, BlockArray &blockArray, ConnectionArray &connectionArray, NetworkGraph &g)
{
    enum State
    {
        Header,
        Blocks,
        Nets,
        End
    };

    std::ifstream myfile;
    myfile.open(filename);
    if(!myfile.is_open())
        return false;

    State state = Header;
    std::string line;
    while(getline(myfile,line))
    {
        if(line.length() == 0 || line[0] == '#')
            continue;
        switch(state)
        {
        case Header:
            if (line == "-block")
                state = Blocks;
            break;
        case Blocks:
            if(line == "-nets")
                state = Nets;
            else
                if (!parseBlockLine(line, blockArray, connectionArray, g)) return false;
            break;
        case Nets:
            if(line == "-end")
                state = End;
            else
                parseNetLine(line, blockArray, connectionArray);
            break;
        case End:
            break;
        }
    }
    return true;
}

bool parseBlockLine(const std::string &line, BlockArray &blockArray, ConnectionArray &connectionArray, NetworkGraph &g)
{
    std::vector<std::string> buf;
    std::string text = line;
    char_separator<char> sep("\t ");
    tokenizer< char_separator<char> > tokens(text, sep);
    BOOST_FOREACH (const std::string& t, tokens)
    {
        buf.push_back(t);
    }
    std::string component_type = buf[0];
    buf.erase(buf.begin());
    if(component_type == "G")
        parseGainBlock(buf, blockArray, connectionArray, g);
    else if(component_type == "S")
        parseMultiPortBlock(buf, blockArray, connectionArray, g);
    return true;
}

void parseGainBlock(std::vector<std::string> &buf, BlockArray &blockArray, ConnectionArray &connectionArray, NetworkGraph &g)
{
    std::string gain = boost::regex_replace(buf[1], boost::regex("[^0-9.]*([0-9.] )*"), std::string("\\1"));
    std::string nf = boost::regex_replace(buf[2], boost::regex("[^0-9.]*([0-9.] )*"), std::string("\\1"));

    if(nf.empty()) nf = "0";

    if(GainBlock *gb = new GainBlock(buf[0], std::stod(gain), std::stod(nf)))
    {
        blockArray.push_back(gb);
        blockArray.back()->descriptor_ = boost::add_vertex(BlockProperty{gb}, g);
    }
}

void parseMultiPortBlock(std::vector<std::string> &buf, BlockArray &blockArray, ConnectionArray &connectionArray, NetworkGraph &g)
{
    std::string gain = boost::regex_replace(buf[1], boost::regex("[^0-9.]*([0-9.] )*"), std::string("\\1"));
    std::string nf = boost::regex_replace(buf[2], boost::regex("[^0-9.]*([0-9.] )*"), std::string("\\1"));
    std::string outputs = boost::regex_replace(buf[3], boost::regex("[^0-9.]*([0-9.] )*"), std::string("\\1"));
    std::string inputs = boost::regex_replace(buf[4], boost::regex("[^0-9.]*([0-9.] )*"), std::string("\\1"));

    if(MultiPortBlock *mp = new MultiPortBlock(buf[0], std::stoi(gain), std::stoi(nf), std::stoi(outputs), std::stoi(inputs)))
    {
        blockArray.push_back(mp);
        blockArray.back()->descriptor_ = boost::add_vertex(BlockProperty{mp}, g);
    }
}

void parseNetLine(const std::string &line, BlockArray &blockArray, ConnectionArray &connectionArray)
{
    std::vector<std::string> tokens;
    std::string text = line;
    char_separator<char> sep("\t .");
    tokenizer< char_separator<char> > line_tokenizer(text, sep);
    BOOST_FOREACH (const std::string& t, line_tokenizer)
    {
        tokens.push_back(t);
    };

    if(Connection *cn = new Connection(tokens[0], stoi(tokens[1]), tokens[2], stoi(tokens[3])))
    {
        connectionArray.push_back(cn);
    }
}

bool drcParsing(BlockArray &blockArray, ConnectionArray &connectionArray)
{
    if(blockArray.empty())
        return false;
    if(connectionArray.empty())
        return false;
    if (!CheckConnectionToMaxNumberOfDriverPorts(blockArray, connectionArray))
        return false;
    if (!CheckConnectionToMaxNumberOfReceiverPorts(blockArray, connectionArray))
        return false;
    if (!CheckIfNetsAreUnique(blockArray, connectionArray))
        return false;

    return true;
}

bool drcGraph(NetworkGraph &g)
{
    if(!GraphContainsNoCycles(g))
        return false;
    if(!Checkconnected_components(g))
        return false;

    return true;
}

//??? verify that each connection has one driver and one receiver
bool CheckIfNetsAreUnique(BlockArray &blockArray, ConnectionArray &connectionArray)
{
    for (size_t cn1 = 0; cn1 < connectionArray.size()-1; ++cn1)
    {
        for(size_t cn2 = cn1+1; cn2 < connectionArray.size(); ++cn2)
        {
            if((connectionArray.at(cn2)->getNameDriver() == connectionArray.at(cn1)->getNameDriver()) &&
              ((connectionArray.at(cn2)->getNumberDriver() == connectionArray.at(cn1)->getNumberDriver())))
            {
                cout << connectionArray.at(cn1)->getNameDriver() << "." << connectionArray.at(cn1)->getNumberDriver()  << " exists twice" << endl;
                return false;
            }
            if((connectionArray.at(cn2)->getNameReceiver() == connectionArray.at(cn1)->getNameReceiver()) &&
              ((connectionArray.at(cn2)->getNumberReceiver() == connectionArray.at(cn1)->getNumberReceiver())))
            {
                cout << connectionArray.at(cn1)->getNameReceiver() << "." << connectionArray.at(cn1)->getNumberReceiver() << " exists twice" << endl;
                return false;
            }
        }
    }
    return true;
}

bool CheckConnectionToMaxNumberOfDriverPorts(BlockArray &blockArray, ConnectionArray &connectionArray)
{
    for (size_t i = 0; i < connectionArray.size(); ++i)
    {
        auto predicate = [&](Block *pBlock){return pBlock->getName() == connectionArray.at(i)->getNameDriver();};
        auto itr = std::find_if(blockArray.begin(), blockArray.end(), predicate);
        if(itr == blockArray.end())
            return false; // block does not exist

        Block *block = *itr;
        if(block->getNumOfOutputs() < connectionArray.at(i)->getNumberDriver())
        {
            cout <<  block->getName() << " has too many outputs\n"; /// ??? was ist genau falsch? "has connection to non existing out?"
            return false;
        }
    }
    return true;
}

bool CheckConnectionToMaxNumberOfReceiverPorts(BlockArray &blockArray, ConnectionArray &connectionArray)
{
    for (size_t i = 0; i < connectionArray.size(); ++i)
    {
        auto predicate = [&](Block *block){return block->getName() == connectionArray.at(i)->getNameReceiver();};
        auto itr = std::find_if(blockArray.begin(), blockArray.end(), predicate);
        if(itr == blockArray.end())
        {
            cout << "block does not exist" << "\n";
            return false; // block does not exist
        }

        Block *block = *itr;
        if(block->getNumOfInputs() < connectionArray.at(i)->getNumberReceiver())
        {
            cout <<  block->getName() << " has too many inputs\n"; /// ??? was ist genau falsch?
            return false;
        }
    }
    return true;
}

void CreateGraph(BlockArray &blockArray, ConnectionArray &connectionArray, NetworkGraph &g)
{
    for (size_t i = 0; i< connectionArray.size(); ++i)
    {
        auto predicateDriver = [&](Block *pBlock){return pBlock->getName() == connectionArray.at(i)->getNameDriver();};
        auto itrDriver = std::find_if(blockArray.begin(), blockArray.end(), predicateDriver);
        Block *block_Driver = *itrDriver;

        auto predicateReceiver = [&](Block *block){return block->getName() == connectionArray.at(i)->getNameReceiver();};
        auto itrReceiver = std::find_if(blockArray.begin(), blockArray.end(), predicateReceiver);
        Block *block_receiver = *itrReceiver;

        boost::add_edge(block_Driver->descriptor_, block_receiver->descriptor_, g);
    }
}

bool GraphContainsNoCycles(NetworkGraph &g)
{
    cycle_detector vis;
    try
    {
       depth_first_search(g, visitor(vis));
    }
    catch (has_cycle)
    {
        std::cout << "Error: cycle detected" << std::endl;
        return false;
    }
    return true;
}

bool Checkconnected_components(NetworkGraph &g) // name
{
     std::vector<int> component (boost::num_vertices (g));
     size_t num_components = boost::connected_components(g, &component[0]);
     if(num_components != 1)
     {
        cout << "vertex with no edge detected\n";/// unconnected blocks ???
        cout << num_components << "\n";
        return false;
     }
     return true;
}

vertex_desc GetSortedVertexDesctiptors(BlockArray &blockArray, NetworkGraph &g)
{
    vertex_desc vertexdescriptors_swap;
    vertex_desc vertexdescriptors;
    getCorrectOrderOfDescriptors vis(vertexdescriptors);
    depth_first_search(g,visitor(vis));
    for(size_t i = vertexdescriptors.size() ; i>0 ; --i)
    {
        vertexdescriptors_swap.push_back(vertexdescriptors.at(i-1));
    }
    return vertexdescriptors_swap;
}

void CalculateGain(BlockArray &blockArray, NetworkGraph &g, vertex_desc &vertexDesc)
{
    string nameoffirstvertex;
    double gain = 0.0;
    for (size_t i = 0; i < vertexDesc.size(); ++i)
    {
        auto desc = vertexDesc.at(i);
        Block *block = g[desc].block_ptr;
        if (i == 0)
            nameoffirstvertex = block->getName();
        gain += block->getElementGain();
        block->getTotalGain() = gain;
        cout << "total gain from " << nameoffirstvertex << " to " << block->getName()<< "\t" << gain << " dB" << "\n";
    }
    return;
}

void CalculateNoiseFigure(BlockArray &blockArray, NetworkGraph &g, vertex_desc &vertexDesc)
{
    string nameOfFirstVertex;
    double total_nf;
    double gain = 1.0;
    for (size_t i = 0; i < vertexDesc.size(); ++i)
    {
        auto desc = vertexDesc.at(i);

        Block *block = g[desc].block_ptr;
        double nf_linear = pow(10, (block->getBlockNF() / 10));
        if (i == 0)
        {
            nameOfFirstVertex = block->getName();
            total_nf = nf_linear;
        }
        else
        {
            total_nf += (nf_linear-1)/gain;
        }
        gain *= pow(10.0, (block->getElementGain() / 10.0));
        block->getTotalNF() = pow(10.0, total_nf);
        cout << "total NF from " << nameOfFirstVertex << " to " << block->getName() << "\t" << block->getTotalNF() << " dB\n";
    }
    return;
}

void CreateOutputFile(const std::string &filename, BlockArray &blockArray, NetworkGraph &g, vertex_desc &vertexDesc)
{
    std::stringstream nameLine;
    std::stringstream gainLine;         //converting to stringstream for auto floatingprecision grow
    std::stringstream noisefigureLine;
    std::stringstream totalGainLine;
    std::stringstream totalNoisefigureLine;

    nameLine <<"Name;";
    gainLine << "Gain;";
    noisefigureLine << "NF;";
    totalGainLine << "Total Gain;";
    totalNoisefigureLine << "Total NF;";

    for(size_t i = 0; i < vertexDesc.size(); ++i)
    {
        auto desc = vertexDesc.at(i);
        Block *block = g[desc].block_ptr;

        nameLine << block->getName();
        gainLine << block->getElementGain();
        noisefigureLine << block->getBlockNF();
        totalGainLine << block->getTotalGain();
        totalNoisefigureLine << block->getTotalNF();

        if(i != vertexDesc.size()-1)
        {
            nameLine << ";";
            gainLine << ";";
            noisefigureLine << ";";
            totalGainLine << ";";
            totalNoisefigureLine << ";";
        }
    }

    std::ofstream outfile(filename);
    outfile << nameLine.str() << endl;
    outfile << gainLine.str() << endl;
    outfile << noisefigureLine.str() << endl;
    outfile << totalGainLine.str() << endl;
    outfile << totalNoisefigureLine.str() << endl;
    outfile << endl;
    outfile.close();

    return;
}

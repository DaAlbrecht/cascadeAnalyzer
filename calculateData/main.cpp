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

typedef boost::adjacency_list< boost::vecS, boost::vecS, boost::bidirectionalS, BlockProperty> NetworkGraph;
typedef std::vector<Block*> BlockArray;
typedef std::vector<Connection*> ConnectionArray;
typedef std::vector <boost::graph_traits<NetworkGraph>::vertex_descriptor> vertex_desc;

bool ReadInputData( const char **argv,BlockArray &myBlockArray, ConnectionArray &myConnectionArray, NetworkGraph &g);

bool parseBlockLine(const std::string &line, BlockArray &myBlockArray, ConnectionArray &myConnectionArray, NetworkGraph &g);
void parseNetLine(const std::string &line, BlockArray &myBlockArray, ConnectionArray &myConnectionArray);
void parseGainBlock(std::vector<std::string> &buf, BlockArray &myBlockArray, ConnectionArray &myConnectionArray, NetworkGraph &g);
void parseMultiPortBlock(std::vector<std::string> &buf, BlockArray &myBlockArray, ConnectionArray &myConnectionArray, NetworkGraph &g);

bool drc_Parsing(BlockArray &myBlockArray, ConnectionArray &myConnectionArray);
bool CheckIfNetsAreUnique(BlockArray &myBlockArray, ConnectionArray &myConnectionArray);
bool CheckConnectionToMaxNumberOfDriverPorts(BlockArray &myBlockArray, ConnectionArray &myConnectionArray);
bool CheckConnectionToMaxNumberOfReceiverPorts(BlockArray &myBlockArray, ConnectionArray &myConnectionArray);
bool CheckifBlocksExists(BlockArray &myBlockArray);
bool CheckifnetsExists(ConnectionArray &myConnectionArray);

void CreatGraph(const char **argv, BlockArray &myBlockArray, ConnectionArray &myConnectionArray, NetworkGraph &g);
bool GraphContainsNoCycles(NetworkGraph &g);
bool Checkconnected_components(NetworkGraph &g);
void GetSortedVertexDesctiptors(const char **argv, BlockArray &myBlockArray, NetworkGraph &g);
void CalculateGain(BlockArray &myBlockArray, NetworkGraph &g, vertex_desc &vertexdescriptors_swap);
void CalculateNoiseFigure(BlockArray &myBlockArray, NetworkGraph &g, vertex_desc &vertexdescriptors_swap);

bool drc_CreatGraph(NetworkGraph &g);

void CreatOutputFile(const char **argv, BlockArray &myBlockArray, NetworkGraph &g, vertex_desc &vertexdescriptors_swap);

class Connection
{
public:
    Connection(std::string &nameDriver, uint8_t numberDriver,std::string &nameReceiver,uint8_t numberReceiver ):
    nameDriver_ (nameDriver),numberDriver_(numberDriver),nameReceiver_(nameReceiver),numberReceiver_(numberReceiver){}

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
    virtual std::string getIdentity()const = 0;
    boost::graph_traits<NetworkGraph>::vertex_descriptor descriptor_;
private:
    std::string name_;
    double totalgain_;
    double totalnf_;
protected:
    Block(const std::string &name):name_(name) {}
    virtual ~Block() {}
};

class GainBlock: public Block
{
public:
    GainBlock(const std::string &name, double gain = 0.0, double nf = 0.0):
        Block(name),
        gain_(gain),
        nf_(nf)
    {}
    virtual double getElementGain()const override
    {
        return gain_;
    }
    virtual double getBlockNF()const override
    {
        return nf_;
    }

    virtual uint8_t getNumOfInputs()const override{return 1;}
    virtual uint8_t getNumOfOutputs()const override{return 1;}
    virtual std::string getIdentity()const override{return "GainBlock";}
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
   virtual double getElementGain()const override
    {
        return gain_;
    }
    virtual double getBlockNF()const override
    {
        return nf_;
    }

    virtual uint8_t getNumOfInputs()const override{ return outputs_;}
    virtual uint8_t getNumOfOutputs()const override{ return inputs_;}
    virtual std::string getIdentity()const override{return "MultiPortBlock";}
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
    if(argc < 3)
    {
        std::cerr  << "Usage:" << argv[0] << "\n" << "ERROR: input_data file or output_data file destination is missing" << "\n";
        std::getchar();
        return -1;
    }

    BlockArray myBlockArray;
    ConnectionArray myConnectionArray;
    NetworkGraph g;

    if(!ReadInputData(argv,myBlockArray, myConnectionArray, g))
    {
        std::cout << "erro parsing input file!\n";
        std::getchar();
        return -1;
    }
    if(!drc_Parsing(myBlockArray, myConnectionArray))
    {
        cout << "drc failed" << endl;
        std::getchar();
        return -1;
    }
    for(auto b : myBlockArray)
    {
        if (b->getIdentity() == "GainBlock")
            std::cout << "name: " << b->getName() << "\t" << " g = " << b->getElementGain() <<" \t"<< " NF = " << b->getBlockNF() <<"\n" ;
        else if (b->getIdentity() == "MultiPortBlock")
            std::cout << "name: " << b->getName() << "\t" << " outputs = " << std::to_string(b->getNumOfOutputs()) << " \t" << "inputs = " << std::to_string(b->getNumOfInputs()) <<"\n";
    }
    for(auto c : myConnectionArray)
    {
       std::cout << "name driver:  " << c->getNameDriver() << "\t" <<"number driver " << std::to_string(c->getNumberDriver()) << "\n";
       std::cout << "name Receiver:" << c->getNameReceiver() << "\t" <<"number Receiver " << std::to_string(c->getNumberReceiver()) << "\n";
    }

    CreatGraph(argv, myBlockArray, myConnectionArray, g);
    if(!drc_CreatGraph(g))
    {
        cout << "drc CreatGraph failed" << endl;
        std::getchar();
        return -1;
    }
    std::getchar();
    return 0;
}

bool ReadInputData(const char **argv,BlockArray &myBlockArray, ConnectionArray &myConnectionArray, NetworkGraph &g)
{
    enum State
    {
        Header,
        Blocks,
        Nets,
        End
    };
    std::ifstream myfile;
    std::string line;
    myfile.open(argv[1]);
    State state = Header;
    if(!myfile.is_open())
        return false;
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
                if (!parseBlockLine(line, myBlockArray, myConnectionArray, g)) return false;
            break;
        case Nets:
            if(line == "-end")
                state = End;
            else
                parseNetLine(line, myBlockArray, myConnectionArray);
            break;
        case End:
            break;
        }
    }
    return true;
}
bool parseBlockLine(const std::string &line, BlockArray &myBlockArray, ConnectionArray &myConnectionArray, NetworkGraph &g)
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
        parseGainBlock(buf, myBlockArray, myConnectionArray, g);
    else if(component_type == "S")
        parseMultiPortBlock(buf, myBlockArray, myConnectionArray, g);
    return true;
}

void parseGainBlock(std::vector<std::string> &buf, BlockArray &myBlockArray, ConnectionArray &myConnectionArray, NetworkGraph &g)
{
    std::string gain = boost::regex_replace(
        buf[1],
        boost::regex("[^0-9.]*([0-9.] )*"),
        std::string("\\1")
    );

      std::string nf = boost::regex_replace(
        buf[2],
        boost::regex("[^0-9.]*([0-9.] )*"),
        std::string("\\1")
    );
    if(nf.empty())
    {
       nf = "0";
    }

    GainBlock *gb = new GainBlock(buf[0],std::stod(gain),std::stod(nf));
    myBlockArray.push_back(gb);
    myBlockArray.back()->descriptor_ = boost::add_vertex(BlockProperty{gb}, g);
}
void parseMultiPortBlock(std::vector<std::string> &buf, BlockArray &myBlockArray, ConnectionArray &myConnectionArray, NetworkGraph &g)
{
    std::string gain = boost::regex_replace(
        buf[1],
        boost::regex("[^0-9.]*([0-9.] )*"),
        std::string("\\1")
    );

      std::string nf = boost::regex_replace(
        buf[2],
        boost::regex("[^0-9.]*([0-9.] )*"),
        std::string("\\1")
    );
      std::string outputs = boost::regex_replace(
        buf[3],
        boost::regex("[^0-9.]*([0-9.] )*"),
        std::string("\\1")
    );
      std::string inputs = boost::regex_replace(
        buf[4],
        boost::regex("[^0-9.]*([0-9.] )*"),
        std::string("\\1")
    );

    MultiPortBlock *mp = new MultiPortBlock(buf[0], std::stoi(gain), std::stoi(nf), std::stoi(outputs), std::stoi(inputs));
    myBlockArray.push_back(mp);
    myBlockArray.back()->descriptor_ = boost::add_vertex(BlockProperty{mp}, g);
}

void parseNetLine(const std::string &line, BlockArray &myBlockArray, ConnectionArray &myConnectionArray)
{
    std::vector<std::string> tokens;
    std::string text = line;
    char_separator<char> sep("\t .");
    tokenizer< char_separator<char> > line_tokenizer(text, sep);
    BOOST_FOREACH (const std::string& t, line_tokenizer)
    {
        tokens.push_back(t);
    };
    Connection *cn = new Connection(tokens[0], stoi(tokens[1]),tokens[2], stoi(tokens[3]));
    myConnectionArray.push_back(cn);
}
bool CheckifBlocksExists(BlockArray &myBlockArray)
{
    if(myBlockArray.empty())
    {
        std::cout << "No existing Blocks" << "\n";
        return false;
    }
    return true;
}
bool CheckifnetsExists(ConnectionArray &myConnectionArray)
{
    if(myConnectionArray.empty())
    {
        std::cout << "No existing Nets" << "\n";
        return false;
    }
    return true;
}
bool drc_Parsing(BlockArray &myBlockArray, ConnectionArray &myConnectionArray)
{
    if(!CheckifBlocksExists(myBlockArray))
        return false;
    if(!CheckifnetsExists(myConnectionArray))
        return false;
    if (!CheckConnectionToMaxNumberOfDriverPorts(myBlockArray, myConnectionArray))
        return false;
    if (!CheckConnectionToMaxNumberOfReceiverPorts(myBlockArray, myConnectionArray))
        return false;
    if (!CheckIfNetsAreUnique(myBlockArray, myConnectionArray))
        return false;

    cout << "ok" << "\n";
    return true;
}
bool drc_CreatGraph(NetworkGraph &g)
{
    if(!GraphContainsNoCycles(g))
        return false;
    if(!Checkconnected_components(g))
        return false;

    return true;
}

bool CheckIfNetsAreUnique(BlockArray &myBlockArray, ConnectionArray &myConnectionArray)
{
    for (size_t i = 0; i < myConnectionArray.size()-1; ++i)
    {
        for(size_t n = i+1; n < myConnectionArray.size(); ++n)
        {
            if((myConnectionArray.at(n)->getNameDriver() == myConnectionArray.at(i)->getNameDriver())
            && ((myConnectionArray.at(n)->getNumberDriver() == myConnectionArray.at(i)->getNumberDriver())))
            {
                cout << myConnectionArray.at(i)->getNameDriver() << "." << myConnectionArray.at(i)->getNumberDriver()  << " exists twice" << endl;
                return false;
            }
            if((myConnectionArray.at(n)->getNameReceiver() == myConnectionArray.at(i)->getNameReceiver())
            && ((myConnectionArray.at(n)->getNumberReceiver() == myConnectionArray.at(i)->getNumberReceiver())))
            {
                cout << myConnectionArray.at(i)->getNameReceiver() <<"." << myConnectionArray.at(i)->getNumberReceiver() << " exists twice" << endl;
                return false;
            }
        }
    }
    return true;
}

bool CheckConnectionToMaxNumberOfDriverPorts(BlockArray &myBlockArray, ConnectionArray &myConnectionArray)
{
    for (size_t i = 0; i < myConnectionArray.size(); ++i)
    {
        auto predicate = [&](Block *pBlock){return pBlock->getName() == myConnectionArray.at(i)->getNameDriver();};
        auto itr = std::find_if(myBlockArray.begin(), myBlockArray.end(), predicate);
        if(itr == myBlockArray.end())
            return false; // block does not exist

        Block *block = *itr;
        if(block->getNumOfOutputs() < myConnectionArray.at(i)->getNumberDriver())
        {
            cout <<  block->getName() << " has too many outputs" << "\n";
            return false;
        }
    }
    return true;
}

bool CheckConnectionToMaxNumberOfReceiverPorts(BlockArray &myBlockArray, ConnectionArray &myConnectionArray)
{
    for (size_t i = 0; i < myConnectionArray.size(); ++i)
    {
        auto predicate = [&](Block *block){return block->getName() == myConnectionArray.at(i)->getNameReceiver();};
        auto itr = std::find_if(myBlockArray.begin(), myBlockArray.end(), predicate);
        if(itr == myBlockArray.end())
        {
            cout << "block does not exist" << "\n";
            return false; // block does not exist
        }

        Block *block = *itr;
        if(block->getNumOfInputs() < myConnectionArray.at(i)->getNumberReceiver())
        {
            cout <<  block->getName() << " has too many inputs" << "\n";
            return false;
        }
    }
    return true;
}

void CreatGraph(const char **argv, BlockArray &myBlockArray, ConnectionArray &myConnectionArray, NetworkGraph &g)
{
    for (size_t i = 0; i< myConnectionArray.size(); ++i)
    {
        auto predicateDriver = [&](Block *pBlock){return pBlock->getName() == myConnectionArray.at(i)->getNameDriver();};
        auto itrDriver = std::find_if(myBlockArray.begin(), myBlockArray.end(), predicateDriver);
        Block *block_Driver = *itrDriver;

        auto predicateReceiver = [&](Block *block){return block->getName() == myConnectionArray.at(i)->getNameReceiver();};
        auto itrReceiver = std::find_if(myBlockArray.begin(), myBlockArray.end(), predicateReceiver);
        Block *block_receiver = *itrReceiver;

        boost::add_edge(block_Driver->descriptor_, block_receiver->descriptor_, g);
    }
    Checkconnected_components(g);
    GraphContainsNoCycles(g);
    GetSortedVertexDesctiptors(argv, myBlockArray, g);
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
bool Checkconnected_components(NetworkGraph &g)
{
     std::vector<int> component (boost::num_vertices (g));
     size_t num_components = boost::connected_components(g, &component[0]);
     if(num_components != 1)
     {
        cout << "vertex with no edge detected" << "\n";
        cout << num_components << "\n";
        return false;
     }
     return true;
}
void GetSortedVertexDesctiptors(const char **argv, BlockArray &myBlockArray, NetworkGraph &g)
{
    std::vector <boost::graph_traits<NetworkGraph>::vertex_descriptor> vertexdescriptors_swap;
    std::vector <boost::graph_traits<NetworkGraph>::vertex_descriptor> vertexdescriptors;
    getCorrectOrderOfDescriptors vis(vertexdescriptors);
    depth_first_search(g,visitor(vis));
    for(size_t i = vertexdescriptors.size(); i>0; --i)
    {
        vertexdescriptors_swap.push_back(vertexdescriptors.at(i-1));
    }
    CalculateGain(myBlockArray, g , vertexdescriptors_swap );
    CalculateNoiseFigure(myBlockArray, g, vertexdescriptors_swap);
    CreatOutputFile(argv, myBlockArray, g, vertexdescriptors_swap);

}
void CalculateGain(BlockArray &myBlockArray, NetworkGraph &g, vertex_desc &vertexdescriptors_swap)
{
    string nameoffirstvertex;
    std::vector<double> Gain;
    Gain.push_back(0);
    for (size_t i = 0; i < vertexdescriptors_swap.size(); ++i)
    {
        auto desc = vertexdescriptors_swap.at(i);
        Block *block = g[desc].block_ptr;
        if (i == 0)
            nameoffirstvertex = block->getName();
        Gain.push_back(block->getElementGain()+ Gain.back());
        block->getTotalGain() = Gain.back();
        cout <<"total gain from " << nameoffirstvertex<< " to " << block->getName()<< "\t" << block->getTotalGain() << " dB" << "\n";
    }
    return;
}

void CalculateNoiseFigure(BlockArray &myBlockArray, NetworkGraph &g,vertex_desc &vertexdescriptors_swap)
{
    string nameoffirstvertex;
    std::vector<double> nf_linear;
    std::vector<double> gain_linear ;
    std::vector<double> nf;
    for (size_t i = 0; i < vertexdescriptors_swap.size(); ++i)
    {
        auto desc = vertexdescriptors_swap.at(i);

        Block *block = g[desc].block_ptr;
        if (i == 0)
        {
            nameoffirstvertex = block->getName();
            nf_linear.push_back(pow(10, (block->getBlockNF() / 10)));
            gain_linear.push_back(pow(10, (block->getElementGain() / 10)));
            nf.push_back(10 * log10(nf_linear.back()));
        }
        else
        {
            nf_linear.push_back(pow(10, (block->getBlockNF() / 10)));
            nf.push_back(10 * log10( pow(10, (nf.at(i -1) / 10)) + ((nf_linear.back() -1)/gain_linear.back())));
            gain_linear.push_back( gain_linear.back() * (pow(10, (block->getElementGain() / 10))));

        }
        block->getTotalNF() = nf.back();
        cout <<"total NF from " << nameoffirstvertex<< " to " << block->getName()<< "\t" << block->getTotalNF() << " dB" << "\n";
    }
    return;
}

void CreatOutputFile(const char **argv, BlockArray &myBlockArray, NetworkGraph &g, vertex_desc &vertexdescriptors_swap)
{
    std::ofstream outfile(argv[2]);
    outfile <<"Name;";
    for(size_t i = 0; i <  vertexdescriptors_swap.size(); ++i )
    {
        auto desc = vertexdescriptors_swap.at(i);
        Block *block = g[desc].block_ptr;
        if( i == vertexdescriptors_swap.size()-1)
            outfile << block->getName()<<" ";
        else
            outfile << block->getName() << ";" <<" ";
    }
    outfile << endl;
    outfile <<"Gain;";
    for(size_t i = 0; i <  vertexdescriptors_swap.size(); ++i )
    {
        auto desc = vertexdescriptors_swap.at(i);
        Block *block = g[desc].block_ptr;
        if( i == vertexdescriptors_swap.size()-1)
            outfile << block->getElementGain()<<" ";
        else
            outfile << block->getElementGain() << ";" <<" ";
    }
    outfile << endl;
    outfile <<"NF;";
    for(size_t i = 0; i <  vertexdescriptors_swap.size(); ++i )
    {
        auto desc = vertexdescriptors_swap.at(i);
        Block *block = g[desc].block_ptr;
        if( i == vertexdescriptors_swap.size()-1)
            outfile << block->getBlockNF()<<" ";
        else
            outfile << block->getBlockNF() << ";" <<" ";
    }
    outfile << endl;
    outfile <<"Total Gain;";
    for(size_t i = 0; i <  vertexdescriptors_swap.size() ; ++i )
    {
        auto desc = vertexdescriptors_swap.at(i);
        Block *block = g[desc].block_ptr;
        if( i == vertexdescriptors_swap.size()-1)
            outfile << block->getTotalGain()<<" ";
        else
            outfile << block->getTotalGain() << ";" <<" ";
    }
    outfile << endl;
    outfile <<"Total NF;";
    for(size_t i = 0; i <  vertexdescriptors_swap.size(); ++i )
    {
        auto desc = vertexdescriptors_swap.at(i);
        Block *block = g[desc].block_ptr;
        if( i == vertexdescriptors_swap.size()-1)
            outfile << block->getTotalNF()<<" ";
        else
            outfile << block->getTotalNF() << ";" <<" ";
    }
    outfile << endl;
    outfile.close();
    if(std::remove(argv[1]) != 0)
        std::cerr << "Error deleting file" << "\n";
    return;
}

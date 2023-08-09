#include <iostream>
#include <string>
#include <vector>
#include <fstream>
using namespace std;
vector<string> split(const std::string& str,string delim)
{
    vector<string> values;

    size_t k = 0;
    size_t len = str.length();
    size_t lineno = 1;
    string curr;
    for(;k<len;k++)
    {
        char ch = str[k];
        if(ch == '"') //found double quote
        {
          if(k+1 > len)
            throw std::logic_error("Expected characters after '\"' at "+to_string(lineno)+":"+to_string(k));
          k++;
          bool found = false;
          size_t newlineno = lineno;
          size_t j = k;
          for(;j<len;j++)
          {
            if(str[j] == '"')
            {
                found = true;
                break;
            }
            else if(str[j]=='\n')
              newlineno += 1;
          }
          if(!found)
            throw std::logic_error("Expected double quote to match '\"' at "+to_string(lineno)+":"+to_string(k-1));
          curr+= str.substr(k,j -k);
          k = j;
          lineno = newlineno;
          continue;
        }
        else if(ch == delim[0] && k+(int)delim.length()-1 < len  && str.substr(k,delim.length())==delim)
        {
            values.push_back(curr);
            curr = "";
            k+= (int)delim.length()-1;
            continue;
        }
        else
        {
          curr += ch;
        }
    }
    values.push_back(curr);
    return values;
}

//CSV Manipulation Class
//It follows RFC Standard 4180
class CSV
{
private:
  vector<string> headings;
  vector<vector<string>> matrix;

public:
  CSV()
  {
  
  }
  CSV(const CSV& rhs)
  {
    matrix = rhs.matrix;
    headings = rhs.headings;
  }
  CSV& operator=(const CSV& rhs)
  {
    if(&rhs == this)
      return *this;
    matrix = rhs.matrix;
    headings = rhs.headings;
    return *this;
  }
  void loadFromFile(const string& filename)
  {
    ifstream fin(filename);
    if(!fin)
    {
        throw std::logic_error("Error opening file!");
    }
    string content;
    size_t len;
    char ch;
    while(fin.get(ch))
      content+=ch;
    fin.close();
    cout<<(content[content.length()-2] == '\r')<<endl;
    vector<string> rows = split(content,"\r\n");
    
    if(rows.size() == 0) //empty file
      return;
    headings = split(rows[0],",");
    if(rows.size() == 1)//only headings, no data
      return;
    size_t k = 1;
    len = rows.size();
    for(;k<len;k++)
    {
        matrix.push_back(split(rows[k],","));
        if(matrix.back().size() != headings.size())
        {
            throw std::logic_error("Line "+
            to_string(k+1)+
            " has "+
            to_string(matrix.back().size())+
            " cols instead of "+
            to_string(headings.size())
            );
        }
    }

  }
  void addRow()
  {
    matrix.push_back(vector<string>{});
    for(size_t i=1;i<=headings.size();i++)
      matrix.back().push_back("");
  }
  void addRow(vector<string> row)
  {
    if(row.size() != headings.size())
      throw std::logic_error("Column count mismatch!");
    matrix.push_back(row);
  }
  void addColumn(std::string name)
  {
    headings.push_back(name);
    size_t len = matrix.size();
    for(size_t i=0;i<len;i++)
      matrix[i].push_back("");
  }
  vector<string>& operator[](size_t idx)
  {
    if(idx >= matrix.size())
      throw std::range_error("index out of range!");
    return matrix[idx];
  }
  void writeToFile(string name)
  {
    ofstream out(name,ios::out);
    if(!out)
      throw std::logic_error("Error opening file for write");
    size_t len = headings.size();
    for(size_t i=0;i<len;i++)
    {
        if(headings[i].find("\r\n")!=std::string::npos || headings[i].find(',')!=std::string::npos)
          out<<"\""<<headings[i]<<"\"";
        else
          out<<headings[i];
        if(i!=len-1)
          out<<",";
    }
    out<<"\r\n";
    len = matrix.size();
    for(size_t i=0;i<len;i++)
    {
        size_t len2 = matrix[i].size();
        for(size_t j=0;j<len2;j++)
        {
            const string& val = matrix[i][j];
            if(val.find("\r\n")!=std::string::npos || val.find(',')!=std::string::npos)
              out<<"\""<<val<<"\"";
            else
              out<<val;
            if(j!=len2-1)
              out<<",";
        }
        if(i!=len-1)
          out<<"\r\n";
    }
    out.close();
  }
  void display()
  {
    for(auto e: headings)
        cout<<e<<" ";
    cout<<endl;
    cout<<"--"<<matrix.size()<<" rows--"<<endl;
    for(auto a: matrix)
    {
        for(auto b: a)
        {
            cout<<b<<" ";
        }
        cout<<endl;
    }
  }
};
int main()
{
/*  string str = "name,age,sex\r\nShahryar,21,Male\r\nAbdul Arham,20,\"Male\"\r\n";
  vector<string> parts = split(str,"\r\n");
  cout<<parts.size()<<endl;
  int i = 1;
  for(auto part: parts)
  {
    cout<<i<<" "<<part<<endl;
    i++;
  }*/
  CSV csv;
  csv.loadFromFile("demo.csv");
  csv[1][2] = "Email";
  csv.addColumn("degree");
  csv[0][3] = "BSCS";
  csv.addRow(vector<string>{"Hamza","20","Male","BSCS"});
  csv.display();

  csv.writeToFile("out.csv");
}
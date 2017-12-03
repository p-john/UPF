#ifndef STATISTICS_H_INCLUDED
#define STATISTICS_H_INCLUDED

class Statistic{

public:

  class Entry{
    public:
    std::string type;
    std::string value;
    Entry(std::string type, std::string value): type(type), value(value){}
  };

  void add_stat(const Entry entry){entries_.push_back(entry);};
  void stat_export(std::string& output_file){
    std::ofstream file;
//    std::remove(output_file.c_str());
    file.open(output_file, std::ios::app);
    file << "#Statistics Export" << std::endl;
    file << "#Type" << " - " << "Value " << std::endl;
    for(auto i : entries_)
        file << i.type << " - " << i.value << std::endl;
    file.close();
  };
  std::vector<Entry> entries_;


  friend std::ostream& operator<<(std::ostream& out, const Statistic& stat);

};


inline std::ostream& operator<<(std::ostream& out, const Statistic& stat){

  for(auto& entry : stat.entries_)
    out << entry.type << "  " <<  entry.value << std::endl;

  return out;
}


#endif // STATISTICS_H_INCLUDED

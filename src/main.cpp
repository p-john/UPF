#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include "UPF_manager.h"

void create_jumpsets();

int main(int argc, char* argv[]){

    std::string filepath_in, filepath_out,engine_in, engine_out, optimization;
    std::vector<std::unique_ptr<Optimization>> opts;
    typedef std::unique_ptr<Optimization> Opt;
    CuttingType cutting_type = weighted_seg;
    unsigned int binth = 4;
    uint64_t block_size = std::numeric_limits<uint64_t>::max();
    bool complete_transform = false;
    bool log_stats = false;
    bool cross_translate = false;
    uint64_t threshold = 4;
    bool use_hypersplit = false;
    bool threading = false;
    uint16_t cores = 1;
    bool enable_log = false;
    bool use_inline = false;

    // Check for valid arguments

    try{
        if(argc < 7)
            throw std::string("usage");

        if(std::strcmp("-i",argv[1]) == 0){
            filepath_in = argv[2];
            std::ifstream f;
            f.open(filepath_in.c_str(), std::ifstream::in);
            if (!f.good())
                throw std::string("Invalid input file specified");
            if(strcmp(argv[3],"-iptables") == 0 || strcmp(argv[3],"-ipfw") == 0||
               strcmp(argv[3],"-pf") == 0 || strcmp(argv[3],"-cb") == 0 ||
               strcmp(argv[3],"-ipf") == 0)

                engine_in = argv[3];
            else
                throw std::string("Invalid Input Engine specified");
        }
        else if (std::strcmp(argv[1],"jumpsets") == 0){
          create_jumpsets();
          return 0;
        }
        else
            throw std::string("usage");

        if(std::strcmp("-o",argv[4]) == 0){
            filepath_out = argv[5];
            std::ofstream f(filepath_out.c_str());
            if(strcmp(argv[6],"-iptables") == 0 || strcmp(argv[6],"-ipfw") == 0||
               strcmp(argv[6],"-pf") == 0 || strcmp(argv[6],"-cb") == 0 ||
               strcmp(argv[6],"-ipf") == 0 || strcmp(argv[6],"-dump") == 0)

                engine_out = argv[6];
            else
                throw std::string("Invalid Output Engine specified");


            if(engine_in != "-cb" && engine_in != engine_out && engine_out != "-dump"){
                throw std::string("Engines must be the same for input and output");
           }
        }
        else
            throw std::string("usage");
        if(argc > 7){
            if(strcmp("-optimize",argv[7]) == 0){
              for (int i = 8; i < argc; ++i){
                // Simple Redundancy Optimization
                if(strcmp(argv[i],"simple_redundancy") == 0){
                  for (int j = i+1; j < argc; ++j){
                    if(strcmp(argv[j],"--block_size") == 0)
                      block_size = atoi(argv[++j]);
                    else if(strcmp(argv[j],"--complete_transform") == 0)
                      complete_transform = true;
                    else if(strcmp(argv[j],"--multithreaded") == 0){
                      threading = true;
                      if (std::string(argv[j+1]).find_first_not_of("0123456789")
                          == std::string::npos){
                            cores = atoi(argv[++j]);
                        }
                    }
                    else
                      break;
                    i=j;
                  }
                  opts.push_back(Opt(new Simple_Redundancy(block_size,
                                                           complete_transform,
                                                           threading)));
                }
                // Redundancy Removal with Firewall Decision Diagramm
                else if(strcmp(argv[i],"fdd_redundancy") == 0){
                  for (int j = i+1; j < argc; ++j){
                    if(strcmp(argv[j],"--block_size") == 0)
                      block_size = atoi(argv[++j]);
                    else if(strcmp(argv[j],"--complete_transform") == 0)
                      complete_transform = true;
                    else if(strcmp(argv[j],"--multithreaded") == 0){
                      threading = true;
                      if (std::string(argv[j+1]).find_first_not_of("0123456789")
                          == std::string::npos){
                            cores = atoi(argv[++j]);
                        }
                    }
                    else
                        break;
                    i=j;
                  }
                  opts.push_back(Opt(new FDD_Redundancy(block_size,
                                                        complete_transform,
                                                        threading)));
                }
                // Firewall Compressor Optimization
                else if(strcmp(argv[i],"firewall_compressor") == 0){
                  for (int j = i+1; j < argc; ++j){
                    if(strcmp(argv[j],"--block_size") == 0)
                      block_size = atoi(argv[++j]);
                    else if(strcmp(argv[j],"--complete_transform") == 0)
                      complete_transform = true;
                    else if(strcmp(argv[j],"--multithreaded") == 0){
                      threading = true;
                      if (std::string(argv[j+1]).find_first_not_of("0123456789")
                          == std::string::npos){
                            cores = atoi(argv[++j]);
                        }
                    }
                    else
                        break;
                    i=j;
                  }
                  opts.push_back(Opt(new FirewallCompressor(block_size,
                                                            complete_transform,
                                                            threading)));
                }
                // Create disjoint rules
                else if(strcmp(argv[i],"disjoint")== 0){
                  for (int j = i+1; j < argc; ++j){
                    if(strcmp(argv[j],"--block_size") == 0)
                      block_size = atoi(argv[++j]);
                    else if(strcmp(argv[j],"--complete_transform") == 0)
                      complete_transform = true;
                    else if(strcmp(argv[j],"--multithreaded") == 0){
                      threading = true;
                      if (std::string(argv[j+1]).find_first_not_of("0123456789")
                          == std::string::npos){
                            cores = atoi(argv[++j]);
                        }
                    }
                    else
                        break;
                    i=j;
                  }
                  opts.push_back(Opt(new Disjunction(block_size,
                                                     complete_transform,
                                                     threading)));
                }
                // Hypersplit Optimization
                else if(strcmp(argv[i],"hypersplit") == 0){
                  for (int j = i+1; j < argc; ++j){
                    if(strcmp(argv[j],"--weighted-segments") == 0)
                        cutting_type = weighted_seg;
                    else if(strcmp(argv[j],"--equal-segments") == 0)
                        cutting_type = equi_seg;
                    else if(strcmp(argv[j],"--equal-rules") == 0)
                        cutting_type = equi_rule;
                    else if(strcmp(argv[j],"--equal-distant") == 0)
                        cutting_type = equi_dist;
                    else if(strcmp(argv[j],"--complete_transform") == 0)
                        complete_transform = true;
                    else if(strcmp(argv[j],"--binth") == 0)
                        binth = atoi(argv[++j]);
                    else if(strcmp(argv[j],"--block_size") == 0)
                        block_size = atoi(argv[++j]);
                    else if(strcmp(argv[j],"--inline") == 0)
                        use_inline = true;
                   else if(strcmp(argv[j],"--multithreaded") == 0){
                      threading = true;
                      if (std::string(argv[j+1]).find_first_not_of("0123456789")
                          == std::string::npos){
                            cores = atoi(argv[++j]);
                        }
                    }
                    else
                      break;
                    i=j;
                  }
                  opts.push_back(Opt(new Hypersplit(cutting_type,binth,
                                                    block_size, complete_transform,
                                                    threading, use_inline)));
                }
                else if(strcmp(argv[i],"saxpac")== 0){
                  for (int j = i+1; j < argc; ++j){
                    if(strcmp(argv[j],"--complete_transform") == 0)
                        complete_transform = true;
                    else if(strcmp(argv[j],"--threshold") == 0)
                      threshold = atoi(argv[++j]);
                    else if(strcmp(argv[j],"--block_size") == 0)
                        block_size = atoi(argv[++j]);
                    else if(strcmp(argv[j],"--binth") == 0)
                        binth = atoi(argv[++j]);
                    else if(strcmp(argv[j],"--use_hypersplit") == 0)
                        use_hypersplit = true;
                    else if(strcmp(argv[j],"--multithreaded") == 0){
                      threading = true;
                      if (std::string(argv[j+1]).find_first_not_of("0123456789")
                          == std::string::npos){
                            cores = atoi(argv[++j]);
                        }
                    }
                    else
                      break;
                    i=j;
                  }
                  opts.push_back(Opt(new SAXPAC(threshold, binth, use_hypersplit,
                                                complete_transform, threading,
                                                block_size)));
                }
                else if(strcmp(argv[i],"-stats") == 0)
                  log_stats = true;
                else if(strcmp(argv[i],"-log") == 0)
                  enable_log = true;
                else{
                  std::cout << "Unknown Optimize Option:" << argv[i] << std::endl
                  << std::endl;
                  throw std::string("usage");
                }
                }
            }
        }
    }
    catch (const std::string msg){
        if(msg == "usage"){
            std::cout <<
            "Usage: " << std::endl << std::endl <<
            "UPF" << " -i " << "<input_file> "
            << "<[-iptables | -ipfw | -pf | -ipf | -cb]> " << std::endl
            << "    -o " << "<output_file> "
            << "<[-iptables | -ipfw | -pf | -ipf ]> " << std::endl
            << "    -optimize " << std::endl
            << "<[simple_redundancy | fdd_redundancy | firewall_compressor "
            << "| disjoint | hypersplit | saxpac]> "
            << std::endl
            << std::endl << "If -optimize is omitted, no optimization is done"
            << std::endl
            << "Multiple Optimizations are done in order of occurence"
            <<  std::endl
            << "If hypersplit is used, a cutting heuristic, an arbitrary binth size"
            << " and complete transformation can be"
            << "specified with following trailing options:"
            << std::endl << std::endl << "--binth <Number> , "
            << "--block_size <Number> ,"
            << "<[--equal-segments | --equal-rules | --weighted-segments]>" << std::endl
            << "--complete_transform"
            << std::endl <<"Default options: --equal-segments and --binth 4"
            << std::endl << std::endl
            << "Example: UPF -i acl1_1K -cb -o acl_1K_iptables -iptables -optimize"
            << " hypersplit --weighted-segments --binth 16" << std::endl
            << std::endl << "Note:"
            << std::endl << "Engines must be the same for input and output"
            << std::endl << "with the exception of cb" << std::endl
            << std::endl << "Put -stats for statistics in \"<output_file>_stats\""
            << std::endl;
        }
        else
            std::cout << msg;


        return -1;
    }

    // Process

    std::cout << "Processing started of " << filepath_in << std::endl;
    EngineType type_in = invalid;
    EngineType type_out = invalid;

    if(engine_in == "-cb"){
        type_in = cb;
        cross_translate = false;
    }
    else if(engine_in == "-iptables")
        type_in = IPTables;
    else if(engine_in == "-ipfw")
        type_in = ipfw;
    else if(engine_in == "-pf")
        type_in = pf;
    else if(engine_in == "-ipf")
        type_in = ipf;

    if(engine_out == "-iptables")
        type_out = IPTables;
    else if(engine_out == "-ipfw")
        type_out = ipfw;
    else if(engine_out == "-pf")
        type_out = pf;
    else if(engine_out == "-ipf")
        type_out = ipf;
    else if(engine_out == "-dump")
        type_out = dump;

    UPF_Manager manager(threading, cores);
    Statistic stats;
    manager.add_ruleset_from_file(filepath_in,type_in);
    manager.optimize(opts, stats);
    if(log_stats){
      std::string stat_file = filepath_out + "_stats.dat" ;
      stats.stat_export(stat_file);
    }
    std::cout << "Start Exporting to " << engine_out.substr(1) << std::endl;
    manager.export_set_to_file(filepath_out,type_out, cross_translate, enable_log);
    return 0;
}
    // SNIPPET FOR GENERATING JUMPSETS
void create_jumpsets(){
    std::vector<std::string> engines;
    engines.push_back("iptables");
    engines.push_back("ipfw");
    engines.push_back("pf");
    engines.push_back("ipf");



    for(auto& engine : engines){
      EngineType e_type = IPTables;
      if(engine == "iptables")
        e_type = EngineType::IPTables;
      else if (engine == "ipfw")
        e_type = EngineType::ipfw;
      else if (engine == "pf")
        e_type = EngineType::pf;
      else if (engine == "ipf")
        e_type = EngineType::ipf;


      for(int k = 0; k < 1001; k += 10){
        std::string filepath = "./jump_sets/seq_jump_set_" + engine + "_" + std::to_string(k);
        uint64_t num_jumps = k;
        UPF_Manager jump_manager;
        std::vector<std::unique_ptr<UPF_Ruleset>> jump_rulesets;

        for(unsigned int i = 1; i <= num_jumps; ++i){


          std::string rset_name = "JUMP_" + std::to_string(i);
          std::string target = "JUMP_" + std::to_string(i+1);
          if(i == num_jumps)
            target = "FINAL";
          std::unique_ptr<UPF_Ruleset> new_set(
            new UPF_Ruleset(rset_name,freeset));
          UPF_Rule* new_rule(new UPF_Rule());
          new_rule->set_action(Jump(target));
          new_set->add_rule(std::unique_ptr<UPF_Rule>(new_rule));
          jump_rulesets.push_back(std::move(new_set));
        }
        std::unique_ptr<UPF_Ruleset> new_set(
            new UPF_Ruleset("FINAL", freeset));
        UPF_Rule* new_rule(new UPF_Rule());
        Field* new_field = new PortField(Range(100,200),TCP);
        new_rule->set_field(l4_src,TCP, new_field);
        new_rule->set_action(ACCEPT);
        new_set->add_rule(std::unique_ptr<UPF_Rule>(new_rule));
        jump_rulesets.push_back(std::move(new_set));

        jump_manager.replace_rulesets(jump_rulesets);
        std::cout << " RULES: " << jump_manager.total_rules() << std::endl;
        jump_manager.export_set_to_file(filepath.c_str(), e_type, false, false);

      }

    for(int k = 0; k < 1001; k += 10){
        std::string filepath = "./jump_sets/skip_jump_set_" + engine + "_" + std::to_string(k);
        uint64_t num_jumps = k;
        UPF_Manager jump_manager;
        std::vector<std::unique_ptr<UPF_Ruleset>> jump_rulesets;
        std::unique_ptr<UPF_Ruleset> new_set(
            new UPF_Ruleset("FIRST", freeset));
        UPF_Rule* new_rule(new UPF_Rule());
          new_rule->set_action(Jump("FINAL"));
          new_set->add_rule(std::unique_ptr<UPF_Rule>(new_rule));
        jump_rulesets.push_back(std::move(new_set));
          std::unique_ptr<UPF_Ruleset> skip_set(
            new UPF_Ruleset("SKIP", freeset));
          for(unsigned int i = 1; i <= num_jumps; ++i){
          Field* new_field = new PortField(Range(i,i),UDP);
          std::unique_ptr<UPF_Rule> new_rule(new UPF_Rule());
          new_rule->set_field(l4_src, new_field);
          new_rule->set_action(ACCEPT);
          skip_set->add_rule(std::move(new_rule));
        }
        jump_rulesets.push_back(std::move(skip_set));
        std::unique_ptr<UPF_Ruleset> final_set(
            new UPF_Ruleset("FINAL", freeset));
        UPF_Rule* final_rule(new UPF_Rule());
        Field* new_field = new PortField(Range(100,200),TCP);
        final_rule->set_field(l4_src,TCP, new_field);
        final_rule->set_action(ACCEPT);
        final_set->add_rule(std::unique_ptr<UPF_Rule>(final_rule));
        jump_rulesets.push_back(std::move(final_set));

        jump_manager.replace_rulesets(jump_rulesets);
        std::cout << " RULES: " << jump_manager.total_rules() << std::endl;
        jump_manager.export_set_to_file(filepath.c_str(), e_type, false, false);
      }

      for(int k = 0; k < 1001; k += 10){
        std::string filepath = "./jump_sets/non_jump_set_" + engine + "_" + std::to_string(k);
        uint64_t num_jumps = k;
        UPF_Manager jump_manager;
        std::vector<std::unique_ptr<UPF_Ruleset>> jump_rulesets;
        std::unique_ptr<UPF_Ruleset> new_set(
            new UPF_Ruleset("FINAL",freeset));

        for(unsigned int i = 1; i <= num_jumps; ++i){
          Field* new_field = new PortField(Range(i,i),UDP);
          std::unique_ptr<UPF_Rule> new_rule(new UPF_Rule());
          new_rule->set_field(l4_src, new_field);
          new_rule->set_action(ACCEPT);
          new_set->add_rule(std::move(new_rule));
        }
        UPF_Rule* new_rule(new UPF_Rule());
        Field* new_field = new PortField(Range(100,200),TCP);
        new_rule->set_field(l4_src,TCP, new_field);
        new_rule->set_action(ACCEPT);
        new_set->add_rule(std::unique_ptr<UPF_Rule>(new_rule));
        jump_rulesets.push_back(std::move(new_set));

        jump_manager.replace_rulesets(jump_rulesets);
        std::cout << " RULES: " << jump_manager.total_rules() << std::endl;
        jump_manager.export_set_to_file(filepath.c_str(), e_type, false, false);

      }
    }

}

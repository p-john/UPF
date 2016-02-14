#include "UPF_ruleset.h"

// UPF_Ruleset

UPF_Ruleset::UPF_Ruleset()
  :   name_("DEFAULT"),
      type_(default_set),
      transformable_(false){}

UPF_Ruleset::UPF_Ruleset(const std::string name, const UPF_setType type)
    : name_(name),
      type_(type),
      transformable_(false){}

UPF_Ruleset::UPF_Ruleset(UPF_Ruleset&& rhs)
  : name_(rhs.name_),
    type_(rhs.type_),
    ruleset_(std::move(rhs.ruleset_)),
    transformable_(rhs.transformable_){}


UPF_setType UPF_Ruleset::get_type() const{return type_;}

void UPF_Ruleset::set_type(UPF_setType type){type_ = type;}

void UPF_Ruleset::add_rule(std::unique_ptr<UPF_Rule> rule){
  ruleset_.push_back(std::move(rule));
}

void UPF_Ruleset::insert_rule(std::unique_ptr<UPF_Rule> rule, uint64_t position){
  ruleset_.insert(ruleset_.begin() + position, std::move(rule));
}

std::unique_ptr<UPF_Rule> UPF_Ruleset::draw_rule(const unsigned int position){

  return std::move(ruleset_[position]);
}

//std::shared_ptr<UPF_Rule> UPF_Ruleset::get_upf_rule(unsigned int position) const{
//  return std::make_shared<UPF_Rule>(*ruleset_[position]);
//}

void UPF_Ruleset::replace_ruleset(std::vector<std::unique_ptr<UPF_Rule>>& rhs){

  ruleset_.clear();
  for(unsigned int i= 0; i < rhs.size(); ++i)
    ruleset_.push_back(std::move(rhs[i]));
}



void UPF_Ruleset::clear(){
  ruleset_.clear();
}

void UPF_Ruleset::set_name(std::string name){
  name_ = name;
}

std::string UPF_Ruleset::get_name() const{
  return name_;
}

unsigned long UPF_Ruleset::size() const {
  return ruleset_.size();
}

void UPF_Ruleset::split_and_remove_redundancy(){

    // Create Free and Legacy Subsets
    std::vector<std::shared_ptr<UPF_Ruleset>> rulesets;
    std::shared_ptr<UPF_Ruleset> new_set(new UPF_Ruleset());
    bool free_mode = false;
    for (auto iter = ruleset_.begin(); iter != ruleset_.end(); ++iter){
        if ((*iter)->get_type() == freerule && free_mode == false){
            if (new_set->size() > 0)
                rulesets.push_back(std::move(new_set));
            new_set = std::shared_ptr<UPF_Ruleset>(new UPF_Ruleset());
            new_set->set_type(freeset);
            free_mode = true;
            new_set->clear();
            new_set->add_rule(std::move(*iter));
        }
        else if((*iter)->get_type() == freerule && free_mode == true){
            new_set->add_rule(std::move(*iter));
        }
        else if ((*iter)->get_type() == legacyrule && free_mode == true){
            if(new_set->size() > 0)
                rulesets.push_back(std::move(new_set));
            new_set = std::shared_ptr<UPF_Ruleset>(new UPF_Ruleset());
            new_set->set_type(legacyset);
            free_mode = false;
            new_set->clear();
            new_set->add_rule(std::move(*iter));
        }
        else if((*iter)->get_type() == legacyrule && free_mode == false)
            new_set->add_rule(std::move(*iter));
    }

    if(free_mode == true)
        rulesets.push_back(std::move(new_set));
    else
        rulesets.push_back(std::move(new_set));

    // Remove redundancy in freesets individually
    std::vector<std::thread*> threads;
    for (auto iter = rulesets.begin(); iter != rulesets.end(); ++iter){

        //create new thread for every subset
      if ((*iter)->get_type() == freeset)
        threads.push_back(new std::thread(&UPF_Ruleset::remove_redundancy,(*iter)));
    }

    // wait for all threads to complete processing
    for (unsigned int i  = 0; i < threads.size() ; ++i){
        threads[i]->join();
        delete(threads[i]);
    }

    // Remerge Free and Legecy sets into one set
    ruleset_.clear();
    for (unsigned int i = 0; i < rulesets.size(); ++i){
        this->append(rulesets[i]);
    }
}
//
//void UPF_Ruleset::split_and_remove_redundancy_mt(Statistic& stats){
//
//    uint64_t count_before = ruleset_.size();
//    auto start = std::chrono::system_clock::now();
//
//   // Create Free and Legacy Subsets
//    std::vector<std::shared_ptr<UPF_Ruleset>> rulesets;
//    std::shared_ptr<UPF_Ruleset> new_set(new UPF_Ruleset());
//
//    bool free_mode = false;
//    for (auto iter = ruleset_.begin(); iter != ruleset_.end(); ++iter){
//        if ((*iter)->get_type() == freerule && free_mode == false){
//            if (new_set->size() > 0)
//                rulesets.push_back(std::move(new_set));
//            new_set = std::shared_ptr<UPF_Ruleset>(new UPF_Ruleset());
//            new_set->set_type(freeset);
//            free_mode = true;
//            new_set->clear();
//            new_set->add_rule(std::move(*iter));
//        }
//        else if((*iter)->get_type() == freerule && free_mode == true){
//            new_set->add_rule(std::move(*iter));
//        }
//        else if ((*iter)->get_type() == legacyrule && free_mode == true){
//            if(new_set->size() > 0)
//                rulesets.push_back(std::move(new_set));
//            new_set = std::shared_ptr<UPF_Ruleset>(new UPF_Ruleset());
//            new_set->set_type(legacyset);
//            free_mode = false;
//            new_set->clear();
//            new_set->add_rule(std::move(*iter));
//        }
//        else if((*iter)->get_type() == legacyrule && free_mode == false)
//            new_set->add_rule(std::move(*iter));
//    }
//    if(free_mode == true)
//        rulesets.push_back(std::move(new_set));
//    else
//        rulesets.push_back(std::move(new_set));
//
//    // Remove redundancy in freesets individually
//    std::vector<std::thread*> threads;
//    for (auto iter = rulesets.begin(); iter != rulesets.end(); ++iter){
//
//        //create new thread for every subset
//        if ((*iter)->get_type() == freeset)
//            threads.push_back(new std::thread(&UPF_Ruleset::remove_redundancy_mt,
//                                              (*iter)));
//    }
//
//    // wait for all threads to complete processing
//    for (unsigned int i  = 0; i < threads.size() ; ++i){
//        threads[i]->join();
//        delete(threads[i]);
//    }
//
//    // Remerge Free and Legecy sets into one set
//    ruleset_.clear();
//
//    for (unsigned int i = 0; i < rulesets.size(); ++i){
//        this->append(rulesets[i]);
//    }
//
//    auto end = std::chrono::system_clock::now();
//    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
//    uint64_t difference = count_before - ruleset_.size();
//    Statistic::Entry cnt_bef("Total Number of rules",
//      std::to_string(count_before));
//    Statistic::Entry cnt_aft_abs("Absolute redundant rules removed",
//      std::to_string(difference));
//    Statistic::Entry cnt_aft_rel("Relative redundant rules removed (in %)",
//      std::to_string(100 - (ruleset_.size()*100 / count_before)));
//    Statistic::Entry proc_time("Time for optimization (in ms) ",
//      std::to_string(elapsed.count()));
//    stats.add_stat(cnt_bef);
//    stats.add_stat(cnt_aft_abs);
//    stats.add_stat(cnt_aft_rel);
//    stats.add_stat(proc_time);
//    if(count_before){
//        std::cout << "Rules before removal: " << std::endl << count_before <<
//        std::endl << "Rules after removal: " <<  std::endl << ruleset_.size() <<
//        std::endl << "Redundant rules removed: " << std::endl << difference <<
//        " (" << 100 - (ruleset_.size()*100 / count_before) << "%)"
//        << std::endl;
//    }
//    std::cout << "Time for optimization: " << std:: endl <<
//    elapsed.count() << "ms" << std::endl;
//}

void UPF_Ruleset::append(std::shared_ptr<UPF_Ruleset> ruleset){
    for (unsigned int i = 0; i < ruleset->size(); ++i)
        this->ruleset_.push_back(std::move(ruleset->draw_rule(i)));

}

void UPF_Ruleset::append(UPF_Ruleset& ruleset){
    for (unsigned int i = 0; i < ruleset.size(); ++i)
        this->ruleset_.push_back(std::move(ruleset.draw_rule(i)));
}

void UPF_Ruleset::append(std::vector<UPF_Rule*>& rules){
    for (unsigned int i = 0; i < rules.size(); ++i)
        this->ruleset_.push_back(std::unique_ptr<UPF_Rule>(rules[i]));

}

unsigned int UPF_Ruleset::cut_intersections(const DimensionType field,
                                            const uint128_t pos) const{
    unsigned int counter = 0;
    Range inter(pos,pos);
    for (unsigned int i = 0; i < ruleset_.size(); ++i){
        if(ruleset_[i]->get_field(field) != nullptr){
            if(ruleset_[i]->get_field(field)->get_range().intersect(inter))
                counter++;
        }
        else
            counter++;
    }
    return counter;
}

std::ostream& UPF_Ruleset::print(std::ostream& out) const {
    if (ruleset_.size() > 0)
        for (auto iter = ruleset_.begin(); iter != ruleset_.end();++iter)
            out << **iter;
  return out;
}


bool UPF_Ruleset::empty() const{
    return ruleset_.size() == 0;
}

UPF_Rule& UPF_Ruleset::get_rule(unsigned int position) const{
        return *ruleset_[position];
}

void UPF_Ruleset::remove_redundancy(){

    // Backward Redundancy
    std::vector<std::unique_ptr<UPF_Rule>> red_rules;
    std::vector<unsigned int> positions;
    for(auto iter = ruleset_.begin(); iter != ruleset_.end(); ++iter){
        bool covered = false;
        for (auto iter2 = ruleset_.begin(); iter2 != iter; ++iter2){
            if ((*iter2)->covers(**iter)){
                covered = true;
            }
        }
        if(!covered){
            std::unique_ptr<UPF_Rule> new_rule((*iter)->clone());
            red_rules.push_back(std::move(new_rule));
        }
    }
    this->replace_ruleset(red_rules);

    // Forward Redundancy
    std::vector<std::unique_ptr<UPF_Rule>> forward_rules;
    for (unsigned int i = 0; i < ruleset_.size(); ++i){
        bool redundant = false;
        for (unsigned int k = i + 1; k < ruleset_.size(); ++k){
            if (ruleset_[k]->covers(*ruleset_[i])){
                if(ruleset_[k]->get_action() == ruleset_[i]->get_action()){
                    bool all_same_action = true;
                    for (unsigned int j = i+1; j < k; ++j){
                        if((ruleset_[i]->intersect(*ruleset_[j])))
                            if(ruleset_[j]->get_action()
                               != ruleset_[i]->get_action())
                                all_same_action = false;
                    }
                    if(all_same_action == true)
                        redundant = true;
                }
            }
        }
        if(redundant != true){
          std::unique_ptr<UPF_Rule> new_rule(ruleset_[i]->clone());
          forward_rules.push_back(std::move(new_rule));
        }
    }
    this->replace_ruleset(forward_rules);
}

void UPF_Ruleset::remove_backward_thread(unsigned int thread_id,
                                         unsigned int number_threads,
                                         PositionVector& pos){

    //Backward Redundancy
    for(unsigned int i = thread_id -1; i < ruleset_.size();i=i+number_threads){
        bool covered = false;
        for (unsigned int j = 0; j < i; ++j){
            if (ruleset_[j]->covers(*ruleset_[i])){
                covered = true;
            }
        }
        if(!covered){
            pos.mtx.lock();
            pos.add(i);
            pos.mtx.unlock();
        }
    }
}

void UPF_Ruleset::remove_forward_thread (unsigned int thread_id,
                                         unsigned int number_threads,
                                         PositionVector& pos){

    // Forward Redundancy
    for (unsigned int i = thread_id - 1; i < ruleset_.size(); i=i+number_threads){
        bool redundant = false;
        for (unsigned int k = i + 1; k < ruleset_.size(); ++k){
            if (ruleset_[k]->covers(*ruleset_[i])){
                if(ruleset_[k]->get_action() == ruleset_[i]->get_action()){
                    bool all_same_action = true;
                    for (unsigned int j = i+1; j < k; ++j){
                        if((ruleset_[i]->intersect(*ruleset_[j])))
                            if(ruleset_[j]->get_action()
                               != ruleset_[i]->get_action())
                                all_same_action = false;
                    }
                    if(all_same_action == true)
                        redundant = true;
                }
            }
        }
        if(redundant != true){
            pos.mtx.lock();
            pos.add(i);
            pos.mtx.unlock();
        }
    }
}

void UPF_Ruleset::remove_redundancy_mt(unsigned int number_threads){

    std::vector<std::thread*> threads;
    PositionVector pos;
    for (unsigned int i = 1; i <= number_threads; ++i){
        threads.push_back(new std::thread
                          (&UPF_Ruleset::
                           remove_backward_thread,this,i,number_threads,
                           std::ref(pos)));
    }
    for (unsigned int i = 0; i < threads.size(); ++i){
        threads[i]->join();
        delete (threads[i]);
    }

    std::vector<std::unique_ptr<UPF_Rule>> new_ruleset;
    std::sort(pos.positions.begin(), pos.positions.end());
    for (unsigned int i = 0; i < pos.positions.size(); ++i){
        std::unique_ptr<UPF_Rule> new_rule(ruleset_[pos.positions[i]]->clone());
        new_ruleset.push_back(std::move(new_rule));
    }

    this->replace_ruleset(new_ruleset);
    threads.clear();
    pos.positions.clear();
    new_ruleset.clear();

    for (unsigned int i = 1; i <= number_threads; ++i){
        threads.push_back(new std::thread
                          (&UPF_Ruleset::
                           remove_forward_thread,this,i,number_threads,
                           std::ref(pos)));
    }
    for (unsigned int i = 0; i < threads.size(); ++i){
        threads[i]->join();
        delete (threads[i]);
    }

    std::sort(pos.positions.begin(), pos.positions.end());
    for (unsigned int i = 0; i < pos.positions.size(); ++i){
        std::unique_ptr<UPF_Rule> new_rule(ruleset_[pos.positions[i]]->clone());
        new_ruleset.push_back(std::move(new_rule));
    }
    this->replace_ruleset(new_ruleset);
}


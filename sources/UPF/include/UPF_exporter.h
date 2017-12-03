#ifndef UPF_EXPORTER_H_INCLUDED
#define UPF_EXPORTER_H_INCLUDED
#include "UPF_ruleset.h"

class UPF_Exporter{
public:
    virtual void upf_export(const std::vector<std::unique_ptr<UPF_Ruleset>>& ruleset,
                            const std::string filepath,
                            bool conservative,
                            bool enable_log) const = 0;
    virtual ~UPF_Exporter(){};
};

class IPTables_Exporter : public UPF_Exporter{
public:
    virtual void upf_export(const std::vector<std::unique_ptr<UPF_Ruleset>>& ruleset,
                            const std::string filepath,
                            bool conservative,
                            bool enable_log) const;
};
class ipfw_Exporter : public UPF_Exporter{

public:
    virtual void upf_export(const std::vector<std::unique_ptr<UPF_Ruleset>>& ruleset,
                            const std::string filepath,
                            bool conservative,
                            bool enable_log) const;
};

class pf_Exporter : public UPF_Exporter{
public:
    virtual void upf_export(const std::vector<std::unique_ptr<UPF_Ruleset>>& ruleset,
                            const std::string filepath,
                            bool conservative,
                            bool enable_log) const;

};

class ipf_Exporter : public UPF_Exporter{
public:
    virtual void upf_export(const std::vector<std::unique_ptr<UPF_Ruleset>>& ruleset,
                            const std::string filepath,
                            bool conservative,
                            bool enable_log) const;

};
#endif // UPF_EXPORTER_H_INCLUDED

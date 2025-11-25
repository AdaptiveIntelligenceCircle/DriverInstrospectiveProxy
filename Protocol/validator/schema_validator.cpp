#include "schema_validator.h"
using namespace std; 

#include <fstream> 
#include <iostream> 

// Note : nlohmann:: json does not include a full Json schema validator.. 
// For small projects we can implement basic checks or include a lightweight.. 
// Here we wil implement a minimal check : confirm required fields exist and simple types... 
// Replace with a proper Json Schema validator in production.. 

namespace dip :: security
{
    static bool json_has_key(const nlohmann :: json &j, const string &k)
    {
        return j.find(k) != j.end(); 
    }

    bool SchemaValidator :: load_schema(const string &schema_path)
    {
        ifstream f(schema_path); 
        if (!f.is_open()){
            cout << "Error:" << schema_path << endl;
            schema_loaded_ = false;  
            return false; 
        }
        try
        {
            /* code */
            f >> schema_ ; 
            schema_loaded_ = true;
            return true;  
        }
        catch(const std::exception& e)
        {
            cerr << "Parse error:"  << e.what() << '\n';
            schema_loaded_ = false; 
            return false; 
        }
        
    }

    ValidateResult SchemaValidator :: validate(const nlohmann :: json &doc) const 
    {
        if (!schema_loaded_) return {
            false, "no_schema_loaded"
        }; 

        // minimal required check.. 
        try
        {
            /* code */
            if (schema_.contains("required") and schema_["required"].is_array()){
                for (auto &req : schema_["required"]){
                    string key = req.get<string>(); 
                    if (!json_has_key(doc, key)){
                        string msg  = "missing required key :"  + key; 
                        return {false, msg} ; 
                    }
                }
            }
        }
        catch(const std::exception& e)
        {
            return {
                false, string("Validation_exception:") + e.what()
            };
        }
        return {true, "ok_minimal"}; 
    }

    ValidateResult SchemaValidator :: validate_file(const string &json_path) const 
    {
        ifstream f(json_path); 
        if (!f.is_open())
        {
            cout <<"Error, cannot find path :" << json_path << endl; 
            return {
                false, "cannot open file"
            }; 
            nlohmann :: json doc; 
            try
            {
                /* code */
                f >> doc;                 
            }
            catch(const std::exception& e)
            {
                return {
                    false, string("json_parse_error:") + e.what()
                }; 
            }
            return validate(doc); 
        }
    }
} // namespace dip :: security... 
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "prefapi.h"

#if defined (_WIN32)
#include <io.h>
#endif // _WIN32
#if defined (__GNUC__)
#include <unistd.h>
char* ltoa(long i, char* s, int dummy_radix) {
    sprintf(s, "%ld", i);
    return s;
}
#endif // __GNUC__

static char ProfilePath[260];

#ifdef __N64__
#include <libdragon.h>
#include <eepromfs.h>

// Helper function to simulate fgets from a memory buffer
static char* mem_fgets(char* str, int n, char** stream) {
    if (!*stream || **stream == '\0') return NULL;
    char* s = str;
    while (--n > 0 && **stream) {
        *s = **stream;
        (*stream)++;
        if (*s++ == '\n') break;
    }
    *s = '\0';
    return str;
}
#endif

/***************************************************************************
   GetPrivateProfileString() -
 ***************************************************************************/
static int                        // RETURN: 1 Found, 0 Not Found
GetPrivateProfileString(
    const char* section,          // INPUT : Name of section
    const char* option,           // INPUT : Name of preference option
    const char* def,              // INPUT : Default value for preference
    char* buf,                    // OUTPUT: buffer to place preference  
    int buflen,                   // INPUT : Maximum size of buffer
    const char* file              // INPUT : File to retrieve data from
)
{
    #ifdef __N64__
    char eeprom_data[312];
    memset(eeprom_data, 0, sizeof(eeprom_data));
    
    const char *base = strrchr(file, '/');
    if (!base) base = strrchr(file, '\\');
    base = base ? base + 1 : file;

    if (eepfs_read(base, eeprom_data, sizeof(eeprom_data)) != EEPFS_ESUCCESS) {
        if (def) { strncpy(buf, def, buflen); buf[buflen - 1] = '\0'; }
        else buf[0] = '\0';
        return 0;
    }

    char *line_ptr = eeprom_data;
    char buffer[128];
    char* token;
    short found = 0;
    short len;

    while (!found && mem_fgets(buffer, sizeof(buffer), &line_ptr)) {
        if (buffer[0] == '[') {
            for (len = 0; buffer[len] != '\0' && buffer[len] != ']'; ++len) ;
            if (buffer[len] != ']') continue;
            buffer[len] = '\0';
            if (strcmp(buffer + 1, section) == 0) found = 1;
        }
    }

    while (found && mem_fgets(buffer, sizeof(buffer), &line_ptr)) {
        if (buffer[0] == '[') found = 0;
        else if ((token = strtok(buffer, "=\r\n")) != NULL) {
            if (strcmp(token, option) == 0) {
                token = strtok(NULL, "\r\n");
                if (token) {
                    strncpy(buf, token, buflen);
                    buf[buflen - 1] = '\0';
                } else buf[0] = '\0';
                return 1;
            }
        }
    }

    if (def) { strncpy(buf, def, buflen); buf[buflen - 1] = '\0'; }
    else buf[0] = '\0';
    
    return 0;
    #else
    char buffer[128];
    char* token;
    FILE* fptr;
    short found = 0;
    short len;

    if ((fptr = fopen(file, "r")) != NULL)
    {
        len = strlen(section);
        
        while (!found && fgets(buffer, sizeof(buffer), fptr))
        {
            if (buffer[0] == '[')
            {
                for (len = 0; buffer[len] != '\0' && buffer[len] != ']'; ++len)
                    ;
                
                if (buffer[len] != ']')
                    continue;
                
                buffer[len] = '\0';
                
                if (strcmp(buffer + 1, section) == 0)
                    found = 1;
            }
        }

        while (found && fgets(buffer, sizeof(buffer), fptr))
        {
            if (buffer[0] == '[')
                found = 0;
            
            else if ((token = strtok(buffer, "=\r\n")) != NULL)
            {
                if (strcmp(token, option) == 0)
                {
                    fclose(fptr);
                    token = strtok(NULL, "\r\n");
                    
                    if (token)
                    {
                        strncpy(buf, token, buflen);
                        buf[buflen - 1] = '\0';
                    }
                    else
                        buf[0] = '\0';
                    
                    return 1;
                }
            }
        }
        fclose(fptr);
    }

    if (def)
        strncpy(buf, def, buflen);
    else
        buf[0] = '\0';
    
    return 0;
    #endif
}

/***************************************************************************
   WritePrivateProfileString() -
 ***************************************************************************/
static int                           // RETURN: 1 Success, 0 Failure
WritePrivateProfileString(
    const char* section,             // INPUT : Name of section 
    const char* option,              // INPUT : Name of preference option
    const char* buf,                 // OUTPUT: preference to write
    const char* file                 // INPUT : File to retrieve data from
)
{
    #ifdef __N64__
    char eeprom_data[312];
    memset(eeprom_data, 0, sizeof(eeprom_data));
    
    const char *base = strrchr(file, '/');
    if (!base) base = strrchr(file, '\\');
    base = base ? base + 1 : file;

    eepfs_read(base, eeprom_data, sizeof(eeprom_data));
    
    char new_data[312];
    memset(new_data, 0, sizeof(new_data));
    char* out_ptr = new_data;
    char* in_ptr = eeprom_data;
    
    char line[128];
    short in_section = 0;
    short section_found = 0;
    short option_found = 0;
    
    if (section == NULL || *section == '\0') return 0;

    while (mem_fgets(line, sizeof(line), &in_ptr)) {
        // Keep empty lines
        if (line[0] == '\r' || line[0] == '\n') {
            int len = strlen(line);
            if ((out_ptr - new_data) + len < (int)sizeof(new_data)) {
                strcpy(out_ptr, line);
                out_ptr += len;
            }
            continue;
        }
        
        if (line[0] == '[') {
            if (in_section && !option_found && option && *option != '\0' && buf && *buf != '\0') {
                int len = snprintf(out_ptr, sizeof(new_data) - (out_ptr - new_data), "%s=%s\r\n", option, buf);
                out_ptr += len;
                option_found = 1;
            }
            
            in_section = 0;
            char sec_name[64];
            int i;
            for (i = 0; line[i+1] != '\0' && line[i+1] != ']'; i++) {
                sec_name[i] = line[i+1];
            }
            sec_name[i] = '\0';
            
            if (strcmp(sec_name, section) == 0) {
                in_section = 1;
                section_found = 1;
                if (!option || *option == '\0') continue; // Deleting entire section
            }
        } else if (in_section) {
            if (!option || *option == '\0') continue; // Deleting section items
            
            char opt_name[64];
            int i;
            for (i = 0; line[i] != '\0' && line[i] != '=' && line[i] != '\r' && line[i] != '\n'; i++) {
                opt_name[i] = line[i];
            }
            opt_name[i] = '\0';
            
            if (strcmp(opt_name, option) == 0) {
                option_found = 1;
                if (buf && *buf != '\0') {
                    // Update value
                    int len = snprintf(out_ptr, sizeof(new_data) - (out_ptr - new_data), "%s=%s\r\n", option, buf);
                    out_ptr += len;
                }
                continue; 
            }
        }
        
        // Copy standard lines
        int len = strlen(line);
        if ((out_ptr - new_data) + len < (int)sizeof(new_data)) {
            strcpy(out_ptr, line);
            out_ptr += len;
        }
    }
    
    // Append at the end if not found
    if (!section_found && option && *option != '\0' && buf && *buf != '\0') {
        snprintf(out_ptr, sizeof(new_data) - (out_ptr - new_data), "\r\n[%s]\r\n%s=%s\r\n", section, option, buf);
    } else if (in_section && !option_found && option && *option != '\0' && buf && *buf != '\0') {
        snprintf(out_ptr, sizeof(new_data) - (out_ptr - new_data), "%s=%s\r\n", option, buf);
    }
    
    if (eepfs_write(base, new_data, sizeof(new_data)) == EEPFS_ESUCCESS) return 1;
    return 0;
    #else
    char buffer[128];
    char* token;
    FILE* fptr;
    short found = 0;
    short len;
    int pos1, pos2, pos3;
    int exist_len, new_len, delta;
    int size;
    int cnt;

    if (section == NULL || *section == '\0')
        return 0;

    if ((fptr = fopen(file, "rb+")) == NULL)
    {
        if ((fptr = fopen(file, "wb+")) == NULL)
            return 0;
        
        fseek(fptr, 0, SEEK_SET);
    }

    // Search for section
    pos3 = 0;
    
    while (!found && fgets(buffer, sizeof(buffer), fptr))
    {
        if (buffer[0] == '\r' || buffer[0] == '\n')
            continue;
        
        if (buffer[0] == '[')
        {
            for (len = 0; buffer[len] != '\0' && buffer[len] != ']'; ++len)
                ;
            
            if (buffer[len] != ']')
                continue;
            
            buffer[len] = '\0';
            
            if (strcmp(buffer + 1, section) == 0)
            {
                found = 1;
                break;
            }
        }
        pos3 = ftell(fptr);
    }
    
    // POS3 = Beginning of section
    if (!found)        // Create the new section
    {
        fseek(fptr, 0, SEEK_END);
        
        if (option && *option != '\0')
            fprintf(fptr, "\r\n[%s]\r\n", section);
        else
        {
            fclose(fptr);
            return 1;   // Delete section that doesn't exist
        }

        found = 0;
        pos1 = pos2 = ftell(fptr);
    }
    else
    {   // Search section for option
        pos1 = ftell(fptr);
        
        while (fgets(buffer, sizeof(buffer), fptr))
        {
            pos2 = ftell(fptr);
            
            if (buffer[0] == '[')   // Option not found
            {
                found = 0;
                break;
            }
            else if ((token = strtok(buffer, "=\r\n")) != NULL)
            {
                if (strcmp(token, option) == 0) // Option found
                {
                    if (strcmp(buf, strtok(NULL, "=\r\n")) == 0)
                    {
                        fclose(fptr);
                        return 1;    // Option value is the same
                    }
                    found = 1;
                    break;
                }
                pos1 = pos2;
            }
        }
    }
    
    // POS3 = Beginning of section
    // POS2 = Current Position after last line read
    // POS1 = Point before current line, the existing option if found
    if (!found)            // Option doesn't exist
    {
        if (pos1 == pos2)  // At end of file, append and exit
        {
            if (option && *option != '\0' && buf && *buf != '\0')
            {
                fseek(fptr, 0, SEEK_END);
                fprintf(fptr, "%s=%s\r\n", option, buf);
            }
            fclose(fptr);
            return 1;
        }
        
        if (option && *option != '\0')
            exist_len = 0;
        else
        {                   // Deleting the section
            exist_len = pos1 - pos3;
            pos2 = pos1;    // Last line read was next section, don't delete
        }
    }
    else
        exist_len = pos2 - pos1;

    if (option && *option != '\0' && buf && *buf != '\0')
    {
        sprintf(buffer, "%s=%s\r\n", option, buf);
        new_len = strlen(buffer);
    }
    else
        new_len = 0;
    
    delta = new_len - exist_len;
    
    if (option && *option != '\0')
        pos3 = pos1;            // Remember where the new line goes

    // POS3 = Beginning of text to remove
    // POS2 = Current Position after text to remove
    // POS1 = Point before current line, the existing option if found
    if (delta < 0)         // Shrink file
    {
        while (1)
        {
            fseek(fptr, pos2, SEEK_SET);
            
            if ((cnt = fread(buffer, sizeof(char), sizeof(buffer), fptr)) == 0)
                break;
            
            pos1 = ftell(fptr);
            fseek(fptr, pos2 + delta, SEEK_SET);
            fwrite(buffer, sizeof(char), cnt, fptr);
            pos2 = pos1;
        }
        fseek(fptr, delta, SEEK_END);
        pos1 = ftell(fptr);
        
        #if defined (_MSC_VER)
        chsize(fileno(fptr), pos1);
        #endif
        #if defined (__GNUC__) && !defined (__N64__)
        ftruncate(fileno(fptr), pos1);
        #endif
    }
    else if (delta > 0)    // Expand file, starting at the end
    {                       // ...working backwards
        size = sizeof(buffer);
        fseek(fptr, 0, SEEK_END);
        pos2 = pos1 = ftell(fptr);
        
        while (1)
        {
            pos2 = pos2 - size;
            
            if (pos2 < pos3)   // Moved to left of line that changed
            {
                size = pos1 - pos3;  // Calculate what is left
                
                if (size <= 0)
                    break;
                
                pos2 = pos3;
            }
            fseek(fptr, pos2, SEEK_SET);
            pos1 = pos2;
            
            if ((cnt = fread(buffer, sizeof(char), (size_t)size, fptr)) == 0)
                break;
            
            fseek(fptr, pos2 + delta, SEEK_SET);
            fwrite(buffer, sizeof(char), cnt, fptr);
        }
    }

    if (option && *option != '\0' && buf && *buf != '\0')
    {
        fseek(fptr, pos3, SEEK_SET);
        fprintf(fptr, "%s=%s\r\n", option, buf);
    }

    fclose(fptr);
    
    return 1;
    #endif
}

/***************************************************************************
   INI_InitPreference() - Specify the preference program file
 ***************************************************************************/
int                             // RETURN: 1 File found, 0 File not found
INI_InitPreference(
    const char* profile         // INPUT : Profile buffer
)
{
    if (profile)
        strcpy(ProfilePath, profile);

    #ifdef __N64__
    const char *base = strrchr(ProfilePath, '/');
    if (!base) base = strrchr(ProfilePath, '\\');
    base = base ? base + 1 : ProfilePath;

    char buffer[312];
    memset(buffer, 0, sizeof(buffer));
    
    if (eepfs_read(base, buffer, sizeof(buffer)) == EEPFS_ESUCCESS) {
        // If the file is completely empty (filled with zeroes on erase), initialize the defaults.
        if (buffer[0] == '\0') {
            const char* default_ini =
                "[Music]\r\n"
                "Volume=0\r\n\r\n"
                "[SoundFX]\r\n"
                "Volume=80\r\n\r\n"
                "[Setup]\r\n"
                "Detail=1\r\n";
                
            strncpy(buffer, default_ini, sizeof(buffer) - 1);
            eepfs_write(base, buffer, sizeof(buffer));
        }
        return 1;
    }
    return 0;
    #else
    if (access(ProfilePath, 04) == 0)
        return 1;
        
    return 0;
    #endif
}

/***************************************************************************
   INI_GetPreferenceLong() - Retrieve a long integer value from the program file
 ***************************************************************************/
int                             // RETURN: Value retrieved
INI_GetPreferenceLong(
    const char* section,        // INPUT : Name of section
    const char* option,         // INPUT : Name of preference option  
    int def                     // INPUT : Default value for preference
)
{
    char buffer[20];
    char Def[20];

    ltoa(def, Def, 10);
    INI_GetPreference(section, option, buffer, sizeof(buffer), Def);
    
    return(atol(buffer));
}

/***************************************************************************
   INI_GetPreferenceHex() - Retrieve a Hex value from the program file
 ***************************************************************************/
int                            // RETURN: Value retrieved
INI_GetPreferenceHex(
    const char* section,       // INPUT : Name of section  
    const char* option,        // INPUT : Name of preference option
    int def                    // INPUT : Default value for preference
)
{
    char buffer[32];
    char Def[32];

    ltoa(def, Def, 10);
    INI_GetPreference(section, option, buffer, sizeof(buffer), Def);
    sscanf(buffer, "%x", &def);
    
    return (def);
}

/***************************************************************************
   INI_GetPreferenceBool() - Retrieve a boolean value from the program file
 ***************************************************************************/
int                             // RETURN: 1 for 1, 0 for 0
INI_GetPreferenceBool(
    const char* section,        // INPUT : Name of section  
    const char* option,         // INPUT : Name of preference option
    short def                   // INPUT : Default value for preference
)
{
    char buffer[10];

    if (def)
        INI_GetPreference(section, option, buffer, sizeof(buffer), "TRUE");
    else
        INI_GetPreference(section, option, buffer, sizeof(buffer), "FALSE");
    
    if (buffer[0] == '1' || strcmp(buffer, "TRUE") == 0)
        return 1;
    
    if (buffer[0] == '0' || strcmp(buffer, "FALSE") == 0)
        return 0;
    
    return(atoi(buffer));
}

/***************************************************************************
   INI_GetPreference() - Retrieve a character string from the program file
 ***************************************************************************/
char*                       // RETURN: Pointer to character buffer
INI_GetPreference(
    const char* section,    // INPUT : Name of section      
    const char* option,     // INPUT : Name of preference option      
    char* buf,              // OUTPUT: buffer to place preference 
    int buflen,             // INPUT : Maximum size of buffer          
    const char* def         // INPUT : Default value for preference
)
{
    if (section == NULL || option == NULL || buf == NULL)
    {
        buf[0] = '\0';
        return NULL;
    }

    GetPrivateProfileString(section, option, def, buf, buflen, ProfilePath);

    return buf;
}

/***************************************************************************
   INI_PutPreferenceLong() - Write a long integer value to the program file
 ***************************************************************************/
int                             // RETURN: SUCCESS 1, FAIL 0
INI_PutPreferenceLong(
    const char* section,        // INPUT : Name of section   
    const char* option,         // INPUT : Name of preference option
    int val                     // INPUT : Value to write                 
)
{
    char buffer[32];

    if (val == -1)
    {
        INI_DeletePreference(section, option);
        return(0);
    }

    ltoa(val, buffer, 10);
    
    return(INI_PutPreference(section, option, buffer));
}

/***************************************************************************
   INI_PutPreferenceHex() - Write a Hex value to the program file
 ***************************************************************************/
int                            // RETURN: SUCCESS 1, FAIL 0
INI_PutPreferenceHex(
    const char* section,       // INPUT : Name of section                 
    const char* option,        // INPUT : Name of preference option       
    int val                    // INPUT : Value to write                 
)
{
    char buffer[32];

    if (val == -1)
    {
        INI_DeletePreference(section, option);
        return(0);
    }

    sprintf(buffer, "%x", val);

    return(INI_PutPreference(section, option, buffer));
}

/***************************************************************************
   INI_PutPreferenceBool() - Write a Boolean value to the program file
 ***************************************************************************/
int                             // RETURN: SUCCESS 1, FAIL 0
INI_PutPreferenceBool(
    const char* section,        // INPUT : Name of section           
    const char* option,         // INPUT : Name of preference option       
    short val                   // INPUT : Value to write               
)
{
    return(INI_PutPreference(section, option, (val) ? "TRUE" : "FALSE"));
}

/***************************************************************************
   INI_PutPreference() - Write a string value to the program file
 ***************************************************************************/
int                         // RETURN: SUCCESS 1, FAIL 0
INI_PutPreference(
    const char* section,    // INPUT : Name of section                
    const char* option,     // INPUT : Name of preference option       
    const char* val         // INPUT : Value to write                  
)
{
    if (section == NULL || option == NULL || val == NULL)
        return 0;

    return(WritePrivateProfileString(section, option, val, ProfilePath));
}

/***************************************************************************
   INI_DeletePreference() - Delete a preference from the program file
 ***************************************************************************/
int                            // RETURN: SUCCESS 1, FAIL 0
INI_DeletePreference(
    const char* section,       // INPUT : Name of section                
    const char* option         // INPUT : Name of preference option
)
{
    if (section == NULL)       // If option is NULL, entire section deleted */
        return 0;

    return(WritePrivateProfileString(section, option, NULL, ProfilePath));
}
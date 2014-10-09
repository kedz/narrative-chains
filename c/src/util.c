#include "util.h"
GFileEnumerator *
g_file_enumerate_children (GFile *file,
                           const char *attributes,
                           GFileQueryInfoFlags flags,
                           GCancellable *cancellable,
                           GError **error);


void 
cu_walk_dir(dir)
    GFile *dir;
{

    GFileType type = g_file_query_file_type (
        dir, G_FILE_QUERY_INFO_NONE, NULL);
    if (type==G_FILE_TYPE_DIRECTORY)
      {    
        printf ("%s\n", g_file_get_path (dir)); 
        GFileEnumerator *child_enum = g_file_enumerate_children (
            dir, NULL, G_FILE_QUERY_INFO_NONE, NULL, NULL);        
         
        GFileInfo *child_info = NULL;
        while ((child_info = g_file_enumerator_next_file (
            child_enum, NULL, NULL))!=NULL)
          {
            GFile *child = g_file_enumerator_get_child (
                child_enum, child_info);
            printf ("%s\n", g_file_get_path (child));
            
          }
      }
}

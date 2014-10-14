#include "util.h"

void
__cu_slurp_dir_contents_file_callback(file, data)
    GFile *file;
    void *data;
{
    GPtrArray *files = (GPtrArray *) data;
    g_ptr_array_add (files, g_file_dup (file));
}


void 
cu_walk_dir(dir, data, dir_callback, file_callback)
    GFile *dir;
    void *data;
    void (*dir_callback) (GFile *, void *);
    void (*file_callback) (GFile *, void *);
{

    GFileType type = g_file_query_file_type (
        dir, G_FILE_QUERY_INFO_NONE, NULL);
    if (type==G_FILE_TYPE_DIRECTORY)
      {    
        GFileEnumerator *child_enum = g_file_enumerate_children (
            dir, NULL, G_FILE_QUERY_INFO_NONE, NULL, NULL);        
         
        GFileInfo *child_info = NULL;
        while ((child_info = g_file_enumerator_next_file (
            child_enum, NULL, NULL))!=NULL)
          {

            const char *name = g_file_info_get_name (child_info);
            GFile *child = g_file_get_child (
                g_file_enumerator_get_container (child_enum), name);

            //GFile *child = g_file_enumerator_get_child (
            //    child_enum, child_info);
            ///free (name);

            type = g_file_query_file_type (
                child, G_FILE_QUERY_INFO_NONE, NULL);

            if (type==G_FILE_TYPE_DIRECTORY) {
                if (dir_callback!=NULL)
                    (*dir_callback) (child, data);
                cu_walk_dir (child, data, dir_callback, file_callback);
            } else if (type==G_FILE_TYPE_REGULAR) {
                if (file_callback!=NULL)
                    (*file_callback) (child, data);         
            }

            g_object_unref (child_info);
            child_info = NULL;
            g_object_unref (child);
            child = NULL;

          }
        g_file_enumerator_close (child_enum, NULL, NULL);
        g_object_unref (child_enum);
        
      }
}

GPtrArray * 
cu_slurp_dir_contents (dir)
    GFile *dir;
{
    GPtrArray *files = g_ptr_array_new ();
    cu_walk_dir (
        dir, (void *) files, NULL, __cu_slurp_dir_contents_file_callback);
    return files;
}

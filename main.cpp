#include <chrono>
#include <expected>
#include <fstream>
#include <print>
#include <vector>
#include <wasm_c_api.h>
#include <wasm_export.h>

using namespace std::chrono_literals;

enum class file_error
{
  not_found,
  size_mismatch
};

auto read_file( const std::string& filename ) -> std::expected< std::vector< std::byte >, file_error >
{
  std::ifstream file( filename, std::ios::binary );
  if( !file )
  {
    return std::unexpected( file_error::not_found );
  }

  file.seekg( 0, std::ios::end );
  auto file_size = file.tellg();
  file.seekg( 0, std::ios::beg );

  std::vector< std::byte > file_content( file_size );
  file.read( reinterpret_cast< char* >( file_content.data() ), file_size );

  if( file.gcount() != file_size )
  {
    return std::unexpected( file_error::size_mismatch );
  }

  return file_content;
}

auto main( int argc, char** argv ) -> int
{
  char error_buf[ 128 ] = "\0";
  wasm_module_t module;
  wasm_module_inst_t module_inst;
  wasm_function_inst_t func;
  wasm_exec_env_t exec_env;
  uint32_t stack_size = 8'092, heap_size = 32'768;

  if( argc < 2 )
  {
    std::print( "please provide wasm file" );
    return 1;
  }

  std::print( "loading file: {}\n", argv[ 1 ] );
  auto bytecode = read_file( argv[ 1 ] );

  if( !bytecode.has_value() )
  {
    switch( bytecode.error() )
    {
      case file_error::not_found:
        std::print( stderr, "could not find file" );
        break;
      case file_error::size_mismatch:
        std::print( stderr, "file size mismatch" );
        break;
      default:
        std::print( stderr, "an unknown error has occurred reading bytecode" );
        break;
    }
    return 1;
  }
  std::print( "loaded with {} bytes\n", bytecode->size() );

  /* initialize the wasm runtime by default configurations */
  wasm_runtime_init();

  /* parse the WASM file from buffer and create a WASM module */
  module = wasm_runtime_load( (uint8_t*)bytecode->data(), bytecode->size(), error_buf, sizeof( error_buf ) );
  if( !module )
  {
    std::print( stderr, "unable to load wasm runtime: {}\n", error_buf );
    return 1;
  }

  /* create an instance of the WASM module (WASM linear memory is ready) */
  module_inst = wasm_runtime_instantiate( module, stack_size, heap_size, error_buf, sizeof( error_buf ) );
  if( !module_inst )
  {
    std::print( stderr, "unable to instantiate wasm runtime: {}\n", error_buf );
    return 1;
  }

  /* lookup a WASM function by its name, the function signature can NULL here */
  func = wasm_runtime_lookup_function( module_inst, "_start" );
  if( !func )
  {
    std::print( stderr, "unable to lookup function main()" );
    return 1;
  }

  /* create an execution environment to execute the WASM functions */
  exec_env = wasm_runtime_create_exec_env( module_inst, stack_size );
  if( !exec_env )
  {
    std::print( stderr, "unable to create wasm runtime execution environment" );
    return 1;
  }

  auto start  = std::chrono::steady_clock::now();
  auto retval = wasm_runtime_call_wasm( exec_env, func, 0, nullptr );
  auto stop   = std::chrono::steady_clock::now();

  if( retval )
  {
    std::print( "returned successfully in {}s\n", ( stop - start ) / 1.0s );
  }
  else
  {
    std::print( "failed exceptionally with: {}, in {}s\n",
                wasm_runtime_get_exception( module_inst ),
                ( stop - start ) / 1.0s );
  }
  return 0;
}

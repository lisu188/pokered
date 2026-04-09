#include "pokered/app/application.hpp"

int main(int argc, char** argv) {
  const pokered::Application app;
  return app.Run(argc, argv);
}

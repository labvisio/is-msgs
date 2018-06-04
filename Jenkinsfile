pipeline {
  agent {
    docker {
      image 'ubuntu:16.04'
    }

  }
  stages {
    stage('Cpp Build') {
      steps {
        sh '''./bootstrap.sh
./build.sh'''
      }
    }
  }
}
pipeline {
    agent any

    stages {

		stage('CMake') {
			steps {
				sh '''
				git submodule update --init --recursive
				mkdir -p build
				cd build
				cmake .. -DCMAKE_BUILD_TYPE=Debug -DWorkspace=workspace
				'''
			}
		}

        stage('Build') {
            steps {
                sh '''
				cd build
                make
                '''
            }
        }
        
        stage('Post Process'){
           
            parallel {   
               
				stage('Publish Doxygen') {
                    when {
                        branch 'master'
                    }
                    steps {
                        sh 'doxygen build/Doxyfile'
                        publishHTML([allowMissing: false, alwaysLinkToLastBuild: false, keepAll: false, reportDir: 'docs', reportFiles: 'index.html', reportName: 'Documentation', reportTitles: ''])
                    }
                }
            
				stage('Test') {
					steps {

						sh '''
						cd build
						rm -rf memchecks
						mkdir -p memchecks
						ctest -T memcheck
						'''
					}
					post {
						always{
							junit '**/build/*.xml'
						}
					}
                }            
            }
        }
        
    }
        
    post {
        always {
            step([$class: 'LogParserPublisher', failBuildOnError: true, parsingRulesPath: '/home/schuetz/rules/cpp_rules', showGraphs: true, useProjectRule: false])
            sh '''
            cd build
            mkdir -p memchecks
            '''
            publishValgrind([pattern: 'build/memchecks/*.memcheck', 
                            failThresholdInvalidReadWrite: '100', failThresholdDefinitelyLost: '100', failThresholdTotal: '100', 
                            unstableThresholdInvalidReadWrite: '50', unstableThresholdDefinitelyLost: '50', unstableThresholdTotal: '50',
                            sourceSubstitutionPaths: '${WORKSPACE}', publishResultsForAbortedBuilds: false, publishResultsForFailedBuilds: false, failBuildOnMissingReports: false, failBuildOnInvalidReports: true])
        }
    }
        
}

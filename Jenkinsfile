pipeline {
    agent any

    parameters {
        booleanParam(defaultValue: false, description: 'Run memory checks', name: 'memcheck')
    }

    stages {

		stage('CMake') {
			steps {
				sh '''
				git submodule update --init --recursive
                mkdir -p build
                cd build
                mkdir -p workspace
                cmake .. -DCMAKE_BUILD_TYPE=Debug -DWorkspace=workspace -DBUILD_CLIENT=FALSE -DINSTALL_EXTERN=TRUE -DBOOST_PATH=~/boost_1_66_0
                '''
			}
		}

		stage('Dependencies'){
			steps {
				sh '''
				cd build
				make Update_dependencies
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
        
        stage('Test') {
            steps {
                sh '''
                cd build
                ctest 
                '''
            }
            post {
                always {
                    junit '**/build/*.xml'
                }
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
            
                stage('Memory Check') {
                    when {
                        expression{ params.memcheck }
                    }
                    steps {
                        sh '''
                        cd build
                        rm -rf memchecks
                        mkdir -p memchecks
                        ctest -T memcheck
                        '''
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

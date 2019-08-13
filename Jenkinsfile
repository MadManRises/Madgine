

 def axisList = [
    [//toolchains
		[
			name : "clang-linux",
			dockerImage : 'schuetzo/linux-test-env:latest'
		],
		[
			name : "clang-android",
			dockerImage : 'schuetzo/linux-test-env:latest'
		],
		[
			name : "emscripten",
			dockerImage : 'schuetzo/linux-test-env:latest'
		]
	],           
    [//configurations
		[
			name : "Debug"
		],
		[
			name : "RelWithDebInfo"
		]
	]
]   

def staticConfigs = [
	[
		name : "OpenGL"
	]
]

def tasks = [:]
def comboBuilder

def staticTask = {
    // builds and returns the task for each combination

    /* Map the entries back to a more readable format
       the index will correspond to the position of this axis in axisList[] */
    def toolchain = it[0]
    def configuration = it[1]
    def staticConfig = it[2]
	
    def name = toolchain.name + '-' + configuration.name + '-' + staticConfig.name
	def parentName = toolchain.name + '-' + configuration.name  

	def staticConfigFile = "../test/${staticConfig.name}.cfg"	

    return {
        // This is where the important work happens for each combination
	    stage ("${name}") {
			stage("generate config") {				
				sh """
				cd ${name}
				if ../${parentName}/bin/MadgineLauncher -t \
				--load-plugins ${staticConfigFile} \
				--export-plugins plugins.cfg \
				--no-plugin-cache ; then
					echo "Success"
				else
					echo "generating failed! Falling back to repository version"
					cp -u ../test/${staticConfig.name}_${toolchain.name}.cfg plugins.cfg
					cp -u ../test/components_${staticConfig.name}_${toolchain.name}.cpp components_plugins.cpp
				fi
				"""
			}
			stage("cmake") {
				sh """
				if ${params.fullBuild}; then
					if [ -d "${name}" ]; then 
						rm -Rf ${name};
					fi
				fi
				mkdir -p ${name}
				cd ${name}
				cmake .. \
				-DCMAKE_BUILD_TYPE=${configuration.name} \
				-DCMAKE_TOOLCHAIN_FILE=~/toolchains/${toolchain.name}.cmake \
				-DPLUGIN_DEFINITION_FILE=plugins.cfg \
				-DBUILD_SHARED_LIBS=OFF
				"""						
			}
			stage("build") {				
				sh """
				cd ${name}
				make all 
				"""				
			}
			docker.image(toolchain.dockerImage).inside {
				stage("Test") {
					sh """
					cd ${name}
					ctest
					"""
				}
			}     
			if (name == "emscripten-RelWithDebInfo-OpenGL") {
				stage("Deploy") {
					sh """
					mkdir -p /var/www/html/latest/${env.BRANCH_NAME}
					cp ${name}/bin/MadgineLauncher.* /var/www/html/latest/${env.BRANCH_NAME}
					"""
				}
			}

        }
    }
}

def task = {
    // builds and returns the task for each combination

    /* Map the entries back to a more readable format
       the index will correspond to the position of this axis in axisList[] */
    def toolchain = it[0]
    def configuration = it[1]
	
    def name = toolchain.name + '-' + configuration.name  


    return {
        // This is where the important work happens for each combination
	    stage ("${name}") {
			if (toolchain.name != "emscripten") {
				stage("cmake") {
					sh """
					if ${params.fullBuild}; then
						if [ -d "${name}" ]; then 
							rm -Rf ${name};
						fi
					fi
					mkdir -p ${name}
					cd ${name}
					cmake .. \
					-DCMAKE_BUILD_TYPE=${configuration.name} \
					-DCMAKE_TOOLCHAIN_FILE=~/toolchains/${toolchain.name}.cmake
					"""
				}
				stage("build") {				
					sh """
					cd ${name}
					make all 
					"""				
				}
				docker.image(toolchain.dockerImage).inside {
					stage("Test") {
						sh """
						cd ${name}
						ctest
						"""
					}
				}           
			} else {
				stage("dummy") {
					sh """
						echo "emscripten plugin build is not supported at the moment!"
					"""
				}
			}

			def staticTasks = [:]
			def fillStatic = { def staticname, def args ->
				staticTasks[staticname] = staticTask(args.collect())
			}
			comboBuilder([staticConfigs.clone()], 2, fillStatic, [toolchain, configuration], [toolchain.name, configuration.name])    

			parallel staticTasks
        }
    }
}

/*
    This is where the magic happens
    recursively work through the axisList and build all combinations
*/
comboBuilder = { def axes, int level, def f, def comboEntry, def comboNames ->
    for ( entry in axes[0] ) {
        comboEntry[level] = entry
		comboNames[level] = entry.name
        if (axes.size() > 1 ) {
            comboBuilder(axes.drop(1), level + 1, f, comboEntry, comboNames)
        }
        else {
            f(comboNames.join("-"), comboEntry)
        }
    }
}

def fill = { def name, def args ->
	tasks[name] = task(args.collect())
}

comboBuilder(axisList, 0, fill, [], [])    

pipeline {
    agent any

	parameters {
        booleanParam(defaultValue: false, description: '', name: 'fullBuild')
    }

	options{
		disableConcurrentBuilds()
	}

    stages {
		stage("checkout") {
			steps{
        		checkout scm
				sh """
				git submodule update --init --recursive
				"""
			}
	    }
        stage ("Multiconfiguration Parallel Tasks") {
	        steps {
			    script {
				    parallel tasks
			    }
	        }
        }
    }

	post {
        always {
            junit '**/*.xml'
		
	    recordIssues enabledForFailure: true, tools: [clang()]
        }
    }
}

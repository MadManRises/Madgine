properties([pipelineTriggers([githubPush()])])

def axisList = [
    [//toolchains
		[
			name : "clang-windows",
			dockerImage : 'schuetzo/linux-test-env:latest',
			args : "-DCMAKE_TOOLCHAIN_FILE=../cmake/toolchains/mingw.cmake"
		],
		[
			name : "clang-osx",
			dockerImage : 'schuetzo/linux-test-env:latest',
			args : "-DENABLE_ARC=False -DDEPLOYMENT_TARGET=11.0"
		],
		[
			name : "clang-ios",
			dockerImage : 'schuetzo/linux-test-env:latest',
			args : "-DCMAKE_TOOLCHAIN_FILE=../cmake/toolchains/ios.cmake -DPLATFORM=SIMULATOR64 -DENABLE_ARC=False -DDEPLOYMENT_TARGET=11.0"
		],
		[
			name : "clang-linux",
			dockerImage : 'schuetzo/linux-test-env:latest',
			args : ""
		],
		[
			name : "clang-android",
			dockerImage : 'schuetzo/linux-test-env:latest',
			args : "-DCMAKE_TOOLCHAIN_FILE=../cmake/toolchains/android.cmake -DANDROID_ABI=x86_64"
		],
		[
			name : "clang-emscripten",
			dockerImage : 'schuetzo/linux-test-env:latest',
			args : "-DCMAKE_TOOLCHAIN_FILE=../cmake/toolchains/emscripten-wasm.cmake"
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

	def staticConfigFile = "../test/${staticConfig.name}_base.cfg"	

    return {
        // This is where the important work happens for each combination
	    stage ("${name}") {
			stage("generate config") {				
				sh """
				if ${params.fullBuild}; then
					if [ -d "${name}" ]; then 
						rm -Rf ${name};
					fi
				fi
				mkdir -p ${name}
				cd ${name}
				if ../${parentName}/bin/MadgineLauncher -t \
				--load-plugins ${staticConfigFile} \
				--export-plugins plugins.cfg \
				--no-plugin-cache ; then
					echo "Success"
				else
					echo "generating failed! Falling back to repository version"
					rsync -u ../test/${staticConfig.name}_tools.cfg plugins_tools.cfg
					rsync -u ../test/components_${staticConfig.name}_tools.cpp components_plugins_tools.cpp
				fi
				"""
			}
			stage("cmake") {
				sh """
				cd ${name}
				cmake .. \
				-DCMAKE_BUILD_TYPE=${configuration.name} \
				-DPLUGIN_DEFINITION_FILE=plugins_tools.cfg \
				-DBUILD_SHARED_LIBS=OFF \
				${toolchain.args} \
				${cmake_args}
				"""						
			}
			stage("build") {				
				sh """
				cd ${name}
				make all
				"""				
			}
			stage("Test") {
				//docker.image(toolchain.dockerImage).inside {
				//	sh """
				//	cd ${name}
				//	ctest --output-on-failure
				//	"""
				//}
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
			if (toolchain.name != "clang-emscripten" && toolchain.name != "clang-ios" && toolchain.name != "clang-android") {
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
					-DBUILD_SHARED_LIBS=ON \
					${toolchain.args} \
					${cmake_args}
					"""
				}
				stage("build") {				
					sh """
					cd ${name}
					make all
					"""				
				}
				stage("Test") {
					//docker.image(toolchain.dockerImage).inside {
					//	sh """
					//	cd ${name}
					//	ctest --output-on-failure
					//	"""
					//}
				}           
			} else {
				stage("dummy") {
					sh """
						echo "plugin build is not supported at the moment!"
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
		booleanParam(defaultValue: false, description: '', name: 'timeTrace')
		booleanParam(defaultValue: false, description: '', name: 'iwyu')
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
					cmake_args = "-DUSE_CMAKE_LOG=1 -DMODULES_ENABLE_TASK_TRACKING=ON"
					if (params.timeTrace){
						cmake_args = cmake_args + "-DCMAKE_CXX_FLAGS=-ftime-trace "
					}
					if (params.iwyu){
						cmake_args = cmake_args + """-DCMAKE_CXX_INCLUDE_WHAT_YOU_USE="/home/jenkins/tools/usr/local/bin/include-what-you-use;-Xiwyu;--pch_in_code;-Xiwyu;--prefix_header_includes=remove" """
					}
				    parallel tasks
			    }
	        }
        }
		stage ("Doxygen") {
			steps {
				sh """
					cd clang-osx-Debug

					make doxygen
				"""
			}
		}
    }

	post {
        always {
			recordIssues enabledForFailure: true, tools: [clang()]

			//junit '**/*.xml'
		}
		success {
			sh """
				mkdir -p /opt/homebrew/var/www/latest/${env.BRANCH_NAME}

				cp -r doc/* /opt/homebrew/var/www

				cp clang-emscripten-RelWithDebInfo-OpenGL/bin/MadgineLauncher_plugins_tools.* /opt/homebrew/var/www/latest/${env.BRANCH_NAME}
			"""
		
			archiveArtifacts artifacts: '*-RelWithDebInfo-*/bin/*, *-RelWithDebInfo-*/data/*'
		}
    }
}

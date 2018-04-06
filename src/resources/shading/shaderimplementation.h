#pragma once

#include "../../uniquecomponent.h"

namespace Engine
{
	namespace Resources
	{
		namespace Shading
		{
			/*
			
			class ShaderFactoryBase
			{
			public:
			    virtual ~ShaderFactoryBase() = default;
			
			    virtual void init() = 0;
			};
			
			template <class T, bool defaultShader = false>
			class ShaderFactory : public UniqueComponent<ShaderFactory<T, defaultShader>, ShaderFactoryBase>{
			public:
			    ShaderFactory() = default;
				virtual ~ShaderFactory() {
					if (mImpl) {
						Ogre::RTShader::ShaderGenerator &shaderGenerator = Ogre::RTShader::ShaderGenerator::getSingleton();
						shaderGenerator.removeSubRenderStateFactory(mImpl.get());
					}
				}
			
			    void init(){
			        Ogre::RTShader::ShaderGenerator &shaderGenerator = Ogre::RTShader::ShaderGenerator::getSingleton();
			        mImpl = OGRE_MAKE_UNIQUE(Impl)();
			        shaderGenerator.addSubRenderStateFactory(mImpl.get());
			
			        if (defaultShader){
			            Ogre::RTShader::RenderState* pMainRenderState =
			                shaderGenerator.createOrRetrieveRenderState(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME).first;
			            //pMainRenderState->reset();
			
			            pMainRenderState->addTemplateSubRenderState(
			                        shaderGenerator.createSubRenderState(sType));
			        }
			    }
			
			    static const Ogre::String &type() {
			        return sType;
			    }
			
			private:
			
			    class Impl : public Ogre::RTShader::SubRenderStateFactory {
			
			    public:
			
			        virtual ~Impl(){
			            destroyAllInstances();
			        }
			
			    protected:
			
			        const Ogre::String &getType() const {
			            return sType;
			        }
			
			        Ogre::RTShader::SubRenderState*	createInstance(Ogre::ScriptCompiler* compiler,
			                                                           Ogre::PropertyAbstractNode* prop, Ogre::Pass* pass, Ogre::RTShader::SGScriptTranslator* translator) override
			        {
			            if (prop->name == T::getIdentifier())
			            {
			                if(prop->values.size() == 1)
			                {
			                    mCurrentMaterial = pass->getParent()->getParent();
			                    return createOrRetrieveInstance(translator);
			                }
			            }
			
			            return NULL;
			        }
			
			
			    protected:
			        Ogre::RTShader::SubRenderState *createInstanceImpl(){
			            return OGRE_NEW T(mCurrentMaterial);
			        }
			
			    private:
			        Ogre::Material* mCurrentMaterial;
			
			    };
			
			private:
			    static const Ogre::String sType;
			
			    Ogre::unique_ptr<Impl> mImpl;
			
			};
			
			
			
			template <class T, bool b>
			const Ogre::String ShaderFactory<T, b>::sType = typeid(T).name();
			
			template <class T, bool b = false>
			class ShaderImplementation : public Ogre::RTShader::SubRenderState {
			public:
			    ShaderImplementation(Ogre::Material *mat) :
			        mMaterial(mat){}
			
			    const Ogre::String & getType() const {
			        return Factory::type();
			    }
			
			    static const Ogre::String & getIdentifier() {
			        return sIdentifier;
			    }
			
			    using Factory = ShaderFactory<T, b>;
			
			protected:
			    Ogre::Pass *getPass(){
			        //mMaterial->load();
					if (mMaterial->getNumTechniques() < 2) return 0;
			        Ogre::Technique *tech = mMaterial->getTechnique(1);
			        if (!tech) return 0;
			        return tech->getPass(0);
			    }
			
			protected:
			    Ogre::Material *mMaterial;
			
			private:
			    static const Ogre::String sIdentifier;
			
			
			
			};
			
			*/
		} // namespace Shading
	} // namespace Resources

	/*#ifdef _MSC_VER
	template OGREMADGINE_EXPORT class UniqueComponentCollector<Resources::Shading::ShaderFactoryBase>;
	#endif*/
} // namespace Core

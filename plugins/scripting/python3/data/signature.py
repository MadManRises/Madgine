import inspect


def get(fn):
	sign = inspect.signature(fn)
	pars = sign.parameters
	return ({pars[k].name : pars[k].annotation for k in pars}, sign.return_annotation)
		